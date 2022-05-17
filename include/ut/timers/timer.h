/******************************************************************************
 * 
 * Copyright (C) 2022 Dmitry Plastinin
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

#ifndef UT_TIMER_H
#define UT_TIMER_H

#include <ut/core/event.h>

namespace UT {

class Timer {
public:
    /**************************************************************************
     * Constructors / Destructors
     *************************************************************************/

    Timer();
    ~Timer();

    /**************************************************************************
     * Public methods
     *************************************************************************/

    void start(unsigned int msec);
    void stop();

    /**************************************************************************
     * Events
     *************************************************************************/

    UT::Event<> onTimeout;

    /**************************************************************************
     * Accessors / Mutators
     *************************************************************************/

    bool oneShot() const;
    void setOneShot(bool value);

    bool started() const;

protected:
    /**************************************************************************
     * Methods (Protected)
     *************************************************************************/

    static int reserveId(Timer* timer);
    static void dispatcherLoop();    

    void createTimer(unsigned int msec);
    void deleteTimer();
    void setTime(unsigned int msec);

    /**************************************************************************
     * Members
     *************************************************************************/

    static bool m_dispatcherRunning;
    static std::mutex m_cdtorMutex;
    static std::mutex m_timerMutex;
    static std::thread* m_dispatcherThread;
    static std::vector<Timer*> m_timerInstances;
    static unsigned int m_counter;

    bool m_oneShot = false;
    bool m_started = false;
    int m_id = -1;
    timer_t m_timerid = timer_t();
}; // class Timer

/******************************************************************************
 * Inline definition: Accessors / Mutators
 *****************************************************************************/

inline bool Timer::started() const { return m_started; }

inline bool Timer::oneShot() const { return m_oneShot; }
inline void Timer::setOneShot(bool value) { m_oneShot = value; }

} // namespace Hlk

#endif // UT_TIMER_H