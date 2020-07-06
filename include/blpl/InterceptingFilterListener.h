#pragma once

#include <functional>
#include <mutex>

#include "ProfilingFilterListener.h"

namespace blpl {

/**
 * @brief FilterListener for providing access to the output data of a filter.
 * Manually decorates the ProfilingFilterListener
 */
class InterceptingFilterListener : public ProfilingFilterListener
{
public:
    InterceptingFilterListener() {}

    /**
     * @brief Immediately executes the given function on the last intercepted
     * output data.
     */
    void doOnLastOutData(std::function<void(const std::any&)> doThis)
    {
        std::unique_lock<std::mutex> lock(m_lastDataMutex);
        doThis(m_lastDataCopy);
    }

    /**
     * @brief Executes the given function once, after the next process finished,
     * on the return data.
     * @note doThis will be executed in the filter's thread! Be sure to keep it
     * light.
     */
    void doOnNextOutData(const std::function<void(const std::any&)>& doThis)
    {
        m_doOnNextData = doThis;
    }

    /// From AbstractFilterListener
    void postProcessCallback(const std::any& out) override
    {
        ProfilingFilterListener::postProcessCallback(out);

        m_doOnNextData(out);
        m_doOnNextData = [](const std::any&) {};

        std::unique_lock<std::mutex> lock(m_lastDataMutex);
        m_lastDataCopy = out;
    }

private:
    std::any m_lastDataCopy;
    std::mutex m_lastDataMutex;
    std::function<void(const std::any&)> m_doOnNextData = [](const std::any&) {
    };
};

} // namespace blpl
