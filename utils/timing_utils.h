#ifndef MUAN_UTILS_TIMING_UTILS_H_
#define MUAN_UTILS_TIMING_UTILS_H_

#include "unitscpp/unitscpp.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace muan {

void sleep_for(Time t);
Time now();
void sleep_until(Time t);
}

#endif  // MUAN_UTILS_TIMING_UTILS_H_
