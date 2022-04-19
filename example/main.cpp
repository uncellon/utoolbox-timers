#include <ut/timers/timer.h>
#include <unistd.h>
#include <iostream>

class A : public UT::Object {
public:
    A(UT::EventLoop *eventLoop) : UT::Object(eventLoop) { }
};

int main(int argc, char *argv[]) {
    UT::Timer tmr1, tmr2, tmr3;
    UT::EventLoop mainLoop;
    A a(&mainLoop);

    tmr1.setOneShot(true);
    tmr2.setOneShot(true);
    tmr3.setOneShot(false);

    tmr1.onTimeout.addEventHandler(&a, [] () { std::cout << "1000\n"; });
    tmr2.onTimeout.addEventHandler(&a, [] () { std::cout << "2000\n"; });
    tmr3.onTimeout.addEventHandler(&a, [] () { std::cout << "4000\n"; });

    tmr1.start(1000);
    tmr2.start(2000);
    tmr3.start(4000);

    sleep(20);
}