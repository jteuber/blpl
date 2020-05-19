#pragma once

#include <memory>

#include "AbstractFilter.h"

namespace blpl {

/**
 * @brief This is the interface for all filters of the pipeline. It takes an
 * input, processes it and produces an output to be processed by the next
 * filter.
 */
template <class InData, class OutData>
class Filter : public AbstractFilter
{
protected:
    /**
     * @brief When implementing this interface, this is the part that's used in
     * the pipeline.
     *
     * @param in Input data for this filter of type InData.
     *
     * @return Output of this filter of type OutData.
     */
    virtual OutData processImpl(InData&& in) = 0;

public:
    Filter() {}

    virtual OutData process(InData&& in)
    {
        std::chrono::time_point start =
            std::chrono::high_resolution_clock::now();

        auto out = processImpl(std::move(in));

        m_wallTime += std::chrono::high_resolution_clock::now() - start;
        ++m_counter;

        return out;
    }

    typedef InData inType;
    typedef OutData outType;
};

/// Convenience type for shared_ptr to filters
template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

} // namespace blpl
