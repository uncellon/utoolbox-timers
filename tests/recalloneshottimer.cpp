#include "ut/timers/timer.h"

#include <iostream>

std::condition_variable cv;
UT::Timer timer;
bool called = false;
unsigned int counter = 0;

void timerHandler1() {
    std::cout << "Timer handler 1 called\n";
    counter++;
    if (counter != 10) {
        timer.start(100);
        return;
    }
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
    timer.start(100);
    std::mutex testMutex;
    std::unique_lock testLock(testMutex);
    cv.wait_for(testLock, std::chrono::seconds(3000));
    if (!called) {
        std::cout << "RecallOneShotTimerTest: failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "RecallOneShotTimerTest: passed!\n";
    return EXIT_SUCCESS;
}