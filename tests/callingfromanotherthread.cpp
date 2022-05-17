#include "ut/timers/timer.h"

#include <iostream>

std::condition_variable cv;
UT::Timer timer;
bool called = false;

void timerHandler1() {
    std::cout << "Timer handler 1 called\n";
    called = true;
    cv.notify_one();
}

class A : public UT::Object {
public:
    A(UT::EventLoop* eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char* argv[]) {
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