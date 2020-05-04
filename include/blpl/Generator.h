#pragma once

#include <chrono>
#include <thread>
#include <vector>

namespace blpl {

using HighResClock     = std::chrono::high_resolution_clock;
using HighResDuration  = std::chrono::duration<double>;
using HighResTimeStamp = std::chrono::time_point<HighResClock, HighResDuration>;

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

namespace std {

template <>
class vector<blpl::Generator>
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
    size_t size() const
    {
        return std::numeric_limits<size_t>::max();
    }

private:
    blpl::Generator m_theGenerator;
};

} // namespace std
