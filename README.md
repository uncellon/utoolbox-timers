# Uncellon's Toolbox Timers

![UToolbox Logo](logo.png)

- [Description](#description)
- [Prerequisites](#prerequisites)
- [Examples](#examples)
    - [Multiple timers](#multiple-timers)
- [License](#license)

## Description

This library provides intuitive implementation of timers based on events. Just set timer mode (how many times the timer should fire: one or many, sets by `setOneShot` method), add event handler to the `onTimeout` event, start timer by calling the `start()` method and stop it by calling `stop()` method if you need.

## Prerequisites

- C++17 or higher
- CMake >= 3.16
- [UToolbox Core](https://github.com/uncellon/utoolbox-core) >= 0.0.10

## Examples

### Multiple timers

```cpp
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
```
## License

<img align="right" src="https://www.gnu.org/graphics/lgplv3-with-text-154x68.png">

The library is licensed under [GNU Lesser General Public License 3.0](https://www.gnu.org/licenses/lgpl-3.0.txt):

Copyright © 2023 Dmitry Plastinin

UToolbox Timers is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as pubblished by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

UToolbox Timers is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser Public License for more details