#pragma once

#include <functional>
#include <mutex>

#include "ProfilingFilterListener.h"

namespace blpl {

class InterceptingFilterListener : public ProfilingFilterListener
{
public:
    InterceptingFilterListener() {}

    void doOnLastOutData(std::function<void(const std::any&)> doThis)
    {
        std::unique_lock lock(m_lastDataMutex);
        doThis(m_lastDataCopy);
    }

    void doOnNextOutData(const std::function<void(const std::any&)>& doThis)
    {
        m_doOnNextData = doThis;
    }

    /// From AbstractFilterListener
    void postProcessCallback(const std::any& out) override
    {
        ProfilingFilterListener::postProcessCallback(out);

        std::unique_lock lock(m_lastDataMutex);
        m_doOnNextData(out);
        m_doOnNextData = [](const std::any&) {};
        m_lastDataCopy = out;
    }

private:
    std::any m_lastDataCopy;
    std::mutex m_lastDataMutex;
    std::function<void(const std::any&)> m_doOnNextData = [](const std::any&) {
    };
};

} // namespace blpl
