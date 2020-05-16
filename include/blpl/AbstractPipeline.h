#pragma once

#include <list>
#include <memory>

#include "AbstractFilterThread.h"

namespace blpl {

class AbstractPipeline
{
public:
    void start()
    {
        for (auto& filter : m_filters) {
            filter->start();
        }
    }

    void stop()
    {
        for (auto& filter : m_filters) {
            filter->stop();
        }
    }

    void reset()
    {
        for (auto& filter : m_filters) {
            filter->reset();
        }
    }

    [[nodiscard]] size_t length() const
    {
        return m_filters.size();
    }

protected:
    std::list<std::shared_ptr<AbstractFilterThread>> m_filters;
};

} // namespace blpl
