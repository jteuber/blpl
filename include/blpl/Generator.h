#pragma once

#include <chrono>
#include <thread>
#include <vector>

namespace blpl {

using HighResClock     = std::chrono::high_resolution_clock;
using HighResDuration  = std::chrono::duration<double>;
using HighResTimeStamp = std::chrono::time_point<HighResClock, HighResDuration>;

/**
 * @brief The Generator class is a way of designating a filter as a producer
 * when used as InData. After starting the pipeline it will continuously be fed
 * new dummy objects, i.e. instances of Generator.
 */
class Generator
{
public:
    explicit Generator(HighResTimeStamp finishAt)
    {
        std::this_thread::sleep_until(finishAt);
    }
    Generator() = default;
};

} // namespace blpl
