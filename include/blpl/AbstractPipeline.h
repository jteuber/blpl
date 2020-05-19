#pragma once

#include <list>
#include <memory>

#include "AbstractFilter.h"
#include "AbstractFilterThread.h"

namespace blpl {

struct FilterMetrics
{
    uint32_t counter;
    std::chrono::duration<double> wallTime;
};

class AbstractPipeline
{
public:
    void start()
    {
        for (auto& filter : m_filterThreads) {
            filter->start();
        }
    }

    void stop()
    {
        for (auto& filter : m_filterThreads) {
            filter->stop();
        }
    }

    void reset()
    {
        for (auto& filter : m_filterThreads) {
            filter->reset();
        }
    }

    void resetFilterMetrics()
    {
        for (auto& filter : m_filters) {
            filter->resetMetrics();
        }
    }

    std::list<FilterMetrics> getMetricsAndReset()
    {
        std::list<FilterMetrics> ret;
        for (auto& filter : m_filters) {
            ret.push_back({filter->counter(), filter->wallTime()});
            filter->resetMetrics();
        }
        return ret;
    }

    [[nodiscard]] size_t length() const
    {
        return m_filters.size();
    }

protected:
    std::list<std::shared_ptr<AbstractFilterThread>> m_filterThreads;
    std::list<std::shared_ptr<AbstractFilter>> m_filters;
};

} // namespace blpl
