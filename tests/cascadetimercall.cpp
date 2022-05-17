#include "ut/timers/timer.h"

#include <iostream>

std::condition_variable cv;
bool called = false;

UT::Timer timers[10000];
int currentTimer = 0;

class A : public UT::Object {
public:
    A(UT::EventLoop* eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char* argv[]) {
    UT::EventLoop mainLoop;
    A a(&mainLoop);

    for (size_t i = 0; i < 9999; ++i) {
        timers[i].setOneShot(true);
        timers[i].onTimeout.addEventHandler(&a, [] () {
            std::cout << "Timer " << currentTimer << " called\n";
            timers[++currentTimer].start(1);
        });
    }
    timers[9999].setOneShot(true);
    timers[9999].onTimeout.addEventHandler(&a, [] () {
        std::cout << "Timer 9999 called\n";
        called = true;
        cv.notify_one();
    });
    timers[0].start(1);

    std::cout << "Timer started\n";
    std::mutex m;
    std::unique_lock lock(m);
    cv.wait_for(lock, std::chrono::seconds(20));
    if (!called) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}