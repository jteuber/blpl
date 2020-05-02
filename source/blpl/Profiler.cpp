#include "blpl/Profiler.h"

namespace blpl {

Profiler::Profiler(const std::string& className)
#ifdef PROFILE
    : m_log(className)
{
    m_log.setSilentMode(true);
}
#else
{
    /*unused*/ (void) className;
}
#endif

void Profiler::startCycle()
{
#ifdef PROFILE
    m_tStart = std::chrono::high_resolution_clock::now();
#endif
}

void Profiler::endCycle()
{
#ifdef PROFILE
    // simple profiling
    std::chrono::high_resolution_clock::time_point tEnd =
        std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span =
        std::chrono::duration_cast<std::chrono::duration<double>>(tEnd -
                                                                  m_tStart);

    m_log << time_span.count() << Log::endl;
#endif
}

} // namespace blpl
