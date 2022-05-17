#include "ut/timers/timer.h"

#include <iostream>
#include <unistd.h>

bool threadsRunning = true;
unsigned int triggerCount = 0;
unsigned int startCount = 0;
unsigned int stopCount = 0;

void timerTimeoutHandler() {
    ++triggerCount;
}

class A : public UT::Object {
public:
    A(UT::EventLoop *eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char* argv[]) {
    UT::EventLoop mainLoop;
    A a(&mainLoop);

    UT::Timer timer;
    timer.onTimeout.addEventHandler(&a, timerTimeoutHandler);

    std::thread thread1([&timer] () {
        while (threadsRunning) {
            timer.stop();
            ++stopCount;
        }
    });

    std::thread thread2([&timer] () {
        while (threadsRunning) {
            timer.start(1);
            ++startCount;
        }
    });

    std::thread thread3([&timer] () {
        while (threadsRunning) {
            timer.stop();
            ++stopCount;
        }
    });

    std::thread thread4([&timer] () {
        while (threadsRunning) {
            timer.start(0);
            ++startCount;
        }
    });

    sleep(10);

    threadsRunning = false;
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    std::cout << "Test passed!\n";
    std::cout << "The timer triggered: " << triggerCount << " times\n";
    std::cout << "The timer has been started: " << startCount << " times\n";
    std::cout << "The timer has been stopped: " << stopCount << " times\n";

    return 0;
}