#include "blpl/AbstractPipeline.h"

namespace blpl {

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

} // namespace blpl
