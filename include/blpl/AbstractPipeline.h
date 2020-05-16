#pragma once

#include <list>
#include <memory>

#include "AbstractFilterThread.h"

namespace blpl {

class AbstractPipeline
{
public:
    void start();
    void stop();
    void reset();

    [[nodiscard]] size_t length() const;

protected:
    std::list<std::shared_ptr<AbstractFilterThread>> m_filters;
};

size_t AbstractPipeline::length() const
{
    return m_filters.size();
}

void AbstractPipeline::start()
{
    for (auto& filter : m_filters) {
        filter->start();
    }
}

void AbstractPipeline::stop()
{
    for (auto& filter : m_filters) {
        filter->stop();
    }
}

void AbstractPipeline::reset()
{
    for (auto& filter : m_filters) {
        filter->reset();
    }
}

} // namespace blpl
