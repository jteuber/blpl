#pragma once

#include "Pipeline_global.h"

#include <chrono>
#include <thread>
#include <vector>

namespace blpl {

using HighResClock     = std::chrono::high_resolution_clock;
using HighResDuration  = std::chrono::duration<double>;
using HighResTimeStamp = std::chrono::time_point<HighResClock, HighResDuration>;

class PIPELINE_EXPORT Generator
{
public:
    explicit Generator(HighResTimeStamp finishAt)
    {
        std::this_thread::sleep_until(finishAt);
    }
    Generator() = default;
};

} // namespace blpl

namespace std {

template <>
class PIPELINE_EXPORT vector<blpl::Generator>
{
public:
    blpl::Generator& operator[](size_t) 
    {
        return m_theGenerator;
    }
    const blpl::Generator& operator[](size_t) const
    {
        return m_theGenerator;
    }

private:
    blpl::Generator m_theGenerator;
};

} // namespace std
