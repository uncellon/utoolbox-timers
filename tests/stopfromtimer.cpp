#include "ut/timers/timer.h"

#include <iostream>

std::condition_variable cv;
bool triggered = false;
UT::Timer timer;

void timerTimeoutHandler() {
    std::cout << "Timer triggered\n";
    timer.stop();
    triggered = true;
    cv.notify_one();
}

class A : public UT::Object {
public:
    A(UT::EventLoop* eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char *argv[]) {
    UT::EventLoop mainLoop;
    A a(&mainLoop);

    timer.onTimeout.addEventHandler(&a, timerTimeoutHandler);
    timer.start(1);

    std::mutex mutex;
    std::unique_lock lock(mutex);
    cv.wait_for(lock, std::chrono::seconds(3), [] () { return triggered; });
    if (!triggered) {
        std::cout << "Test not passed\n";
        return EXIT_FAILURE;
    }

    std::cout << "Test passed\n";
    return EXIT_SUCCESS;
}