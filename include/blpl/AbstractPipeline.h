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

    [[nodiscard]] size_t length() const noexcept
    {
        return m_filters.size();
    }

    const std::list<std::shared_ptr<AbstractFilter>>& filters() const noexcept
    {
        return m_filters;
    }

protected:
    std::list<std::shared_ptr<AbstractFilterThread>> m_filterThreads;
    std::list<std::shared_ptr<AbstractFilter>> m_filters;
};

} // namespace blpl
