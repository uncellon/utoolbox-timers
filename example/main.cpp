#include <iostream>
#include <unistd.h>
#include <ut/timers/timer.h>

int main(int argc, char* argv[]) {
    auto eh = UT::EventLoop::getMainInstance();
    UT::Timer tmr1, tmr2, tmr3;

    tmr1.setOneShot(true);
    tmr2.setOneShot(true);
    tmr3.setOneShot(false);

    tmr1.onTimeout.addEventHandler(eh, [] () { std::cout << "1000\n"; });
    tmr2.onTimeout.addEventHandler(eh, [] () { std::cout << "2000\n"; });
    tmr3.onTimeout.addEventHandler(eh, [] () { std::cout << "4000\n"; });

    tmr1.start(1000);
    tmr2.start(2000);
    tmr3.start(4000);

    sleep(20);
}