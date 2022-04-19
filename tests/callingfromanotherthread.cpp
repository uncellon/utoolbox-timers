#include "ut/timers/timer.h"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <condition_variable>

std::condition_variable cv;
UT::Timer timer;
bool called = false;

void sigsegvHandler(int signum) {
    int nptrs, fd;
    void *buffer[1024];

    nptrs = backtrace(buffer, 1024);
    fd = open("callingfromanotherthread_backtrace.txt", O_CREAT | O_WRONLY | O_TRUNC, 0665);
    backtrace_symbols_fd(buffer, nptrs, fd);
    close(fd);

    signal(signum, SIG_DFL);
    exit(3);
}

void timerHandler1() {
    std::cout << "Timer handler 1 called\n";
    called = true;
    cv.notify_one();
}

class A : public UT::Object {
public:
    A(UT::EventLoop *eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char* argv[]) {
    signal(SIGSEGV, sigsegvHandler);

    UT::EventLoop mainLoop;
    A a(&mainLoop);

    timer.setOneShot(true);
    timer.onTimeout.addEventHandler(&a, timerHandler1);

    {
        auto thr = std::thread([] () {
            timer.start(1);
        });
        thr.detach();
    }
    std::mutex m;
    std::unique_lock lock(m);
    cv.wait_for(lock, std::chrono::seconds(3));
    if (!called) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}