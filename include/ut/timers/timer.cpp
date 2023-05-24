/******************************************************************************
 * 
 * Copyright (C) 2023 Dmitry Plastinin
 * Contact: uncellon@yandex.ru, uncellon@gmail.com, uncellon@mail.ru
 * 
 * This file is part of the UToolbox Timers library.
 * 
 * UToolbox Timers is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as pubblished by 
 * the Free Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * UToolbox Timers is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser Public License for 
 * more details
 * 
 * You should have received a copy of the GNU Lesset General Public License
 * along with UToolbox Timers. If not, see <https://www.gnu.org/licenses/>.
 * 
 *****************************************************************************/

#include "timer.h"

#include <unistd.h>
#include <signal.h>

#define UT_TIMER_SIGNAL SIGRTMIN + 1

namespace UT {

/******************************************************************************
 * Static initialization
 *****************************************************************************/

bool Timer::mDispatcherRunning = false;
std::mutex Timer::mCdtorMutex;
std::mutex Timer::mTimerMutex;
std::thread* Timer::mDispatcherThread = nullptr;
std::vector<Timer*> Timer::mTimerInstances;
unsigned int Timer::mCounter = 0;

/******************************************************************************
 * Functions
 *****************************************************************************/

static void timerSignalHandler(int sig, siginfo_t* info, void* ucontext) {
    sigaddset(&static_cast<ucontext_t*>(ucontext)->uc_sigmask, UT_TIMER_SIGNAL);    
    sigqueue(getpid(), UT_TIMER_SIGNAL, info->_sifields._timer.si_sigval);
}

/******************************************************************************
 * Constructors / Destructors
 *****************************************************************************/

Timer::Timer() {
    std::unique_lock lock(mCdtorMutex);

    if (++mCounter != 1) {
        return;
    }

    mTimerInstances.clear();
    
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, UT_TIMER_SIGNAL);
    sigprocmask(SIG_BLOCK, &sigset, nullptr);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sa.sa_sigaction = timerSignalHandler;
    sigaction(UT_TIMER_SIGNAL, &sa, nullptr);
    raise(UT_TIMER_SIGNAL);

    mDispatcherRunning = true;
    mDispatcherThread = new std::thread(&Timer::dispatcherLoop);
}

Timer::~Timer() {
    stop();

    std::unique_lock lock(mCdtorMutex);

    if (--mCounter != 0) {
        return;
    }

    mDispatcherRunning = false;
    sigqueue(getpid(), UT_TIMER_SIGNAL, sigval());
    mDispatcherThread->join();
    delete mDispatcherThread;
    mDispatcherThread = nullptr;

    mTimerInstances.clear();
}

/******************************************************************************
 * Public methods
 *****************************************************************************/

void Timer::start(unsigned int msec) {
    std::unique_lock lock(mTimerMutex);
    if (!mStarted) {
        mStarted = true;
        createTimer(msec);
    }
    setTime(msec);
}

void Timer::stop() {
    std::unique_lock lock(mTimerMutex);

    if (!mStarted) {
        return;
    }

    mStarted = false;
    deleteTimer();
    mTimerid = timer_t();
    mId = -1;
}

/******************************************************************************
 * Static: Methods
 *****************************************************************************/

inline int Timer::reserveId(Timer* timer) {
    for (size_t i = 0; i < mTimerInstances.size(); ++i) {
        if (mTimerInstances[i] != nullptr) {
            continue;
        }
        mTimerInstances[i] = timer;
        return i;
    }

    mTimerInstances.emplace_back(timer);
    return mTimerInstances.size() - 1;
}

void Timer::dispatcherLoop() {
    Timer* timerInstance = nullptr;
    int index = 0;

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, UT_TIMER_SIGNAL);

    siginfo_t siginfo;

    while (mDispatcherRunning) {
        sigwaitinfo(&sigset, &siginfo);
        
        if (!siginfo._sifields._timer.si_sigval.sival_ptr) {
            continue;
        }

        std::unique_lock lock(mTimerMutex);

        index = siginfo._sifields._timer.si_sigval.sival_int;
        timerInstance = mTimerInstances[index];
        if (!timerInstance || !timerInstance->mStarted) {
            continue;
        }
        timerInstance->onTimeout();
        if (timerInstance->oneShot()) {
            timerInstance->mStarted = false;
            timerInstance->deleteTimer();
        }
    }
}

/******************************************************************************
 * Protected: Methods
 *****************************************************************************/

inline void Timer::createTimer(unsigned int msec) {
    // Reserve signal for this object
    mId = reserveId(this);

    // Create sigevent for timer
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = UT_TIMER_SIGNAL;
    sev.sigev_value.sival_ptr = &mTimerid;
    sev.sigev_value.sival_int = mId;
    sev._sigev_un._sigev_thread._attribute = nullptr;

    if (timer_create(CLOCK_MONOTONIC, &sev, &mTimerid) == -1) {
        throw std::runtime_error("timer_create(...) failed, errno: "
            + std::to_string(errno));
    }
}

inline void Timer::deleteTimer() {
    timer_delete(mTimerid);
    mTimerInstances[mId] = nullptr;
}

inline void Timer::setTime(unsigned int msec) {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    itimerspec its;
    auto nsec = (msec % 1000) * 1000000;
    its.it_value.tv_sec = ts.tv_sec + (msec / 1000) + (ts.tv_nsec + nsec) / 1000000000;
    its.it_value.tv_nsec = (ts.tv_nsec + nsec) % 1000000000;
    if (mOneShot) {
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
    } else {
        its.it_interval.tv_sec = msec / 1000;
        its.it_interval.tv_nsec = (msec % 1000) * 1000000;
    }

    if (timer_settime(mTimerid, TIMER_ABSTIME, &its, nullptr) == -1) {
        throw std::runtime_error("timerfd_settime(...) failed, errno: " 
            + std::to_string(errno));
    }
}

} // namespace Hlk