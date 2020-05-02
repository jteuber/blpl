#pragma once

#include <chrono>
#include <string>

#include "Log.h"

namespace blpl {

/**
 * @brief The Profiler class can be used for simple profiling of cyclic
 * applications. It needs the define PROFILE to be enabled and will be optimized
 * away if that is not defined.
 */
class Profiler
{
public:
    explicit Profiler(const std::string& className);

    void startCycle();
    void endCycle();

#ifdef PROFILE
private:
    Log m_log;

    std::chrono::high_resolution_clock::time_point m_tStart;
#endif
};

}
