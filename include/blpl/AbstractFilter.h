#pragma once

#include <chrono>
#include <cstdint>

namespace blpl {

class AbstractFilter
{
public:
    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     *
     * @note Any implementer needs to call this as well.
     */
    virtual void reset()
    {
        resetMetrics();
    }

    virtual ~AbstractFilter() = default;

    /**
     * @brief Returns the number of runs of the filter since the start of the
     * pipeline or last call to resetCounters().
     */
    uint32_t counter()
    {
        return m_counter;
    }
    /**
     * @brief Returns the (wall) time spend in the filter's process method since
     * the start of the pipeline or last call to resetCounters().
     *
     * @note We don't use CPU time here because std::clock (only method to get
     * CPU times) doesn't behave according to standard on Windows and the
     * multi-threaded nature of the pipeline might make the CPU time meaningless
     * anyway.
     */
    std::chrono::duration<double> wallTime()
    {
        return m_wallTime;
    }
    void resetMetrics()
    {
        m_counter  = 0;
        m_wallTime = std::chrono::duration<double>::zero();
    }

protected:
    uint32_t m_counter = 0;
    std::chrono::duration<double> m_wallTime;
};

} // namespace blpl
