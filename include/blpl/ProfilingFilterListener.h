#pragma once

#include "AbstractFilter.h"

namespace blpl {

class ProfilingFilterListener : public AbstractFilterListener
{
public:
    ProfilingFilterListener() {}

    /**
     * @brief Returns the number of runs of the filter since the start of the
     * pipeline or last call to resetCounters().
     */
    uint32_t counter() const
    {
        return m_counter;
    }

    /**
     * @brief Returns the (wall) time spend in the filter's process method since
     * the start of the pipeline or last call to resetMetrics().
     *
     * @note We don't use CPU time here because std::clock (only method to get
     * CPU times) doesn't behave according to standard on Windows and the
     * multi-threaded nature of the pipeline might make the CPU time meaningless
     * anyway.
     */
    std::chrono::duration<double> wallTime() const
    {
        return m_wallTime;
    }

    /**
     * @brief reset all collected metrics.
     */
    void resetMetrics()
    {
        m_counter  = 0;
        m_wallTime = std::chrono::duration<double>::zero();
    }

    /// From AbstractFilterListener
    void preProcessCallback(const std::any&) override
    {
        m_lastStart = std::chrono::high_resolution_clock::now();
    }
    void postProcessCallback(const std::any&) override
    {
        m_wallTime += std::chrono::high_resolution_clock::now() - m_lastStart;
        ++m_counter;
    }

private:
    uint32_t m_counter = 0;
    std::chrono::duration<double> m_wallTime;
    std::chrono::high_resolution_clock::time_point m_lastStart;
};

} // namespace blpl
