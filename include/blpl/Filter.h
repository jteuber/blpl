#pragma once

#include <memory>

#include "Profiler.h"

namespace blpl {

class AbstractFilter
{
public:
    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset() {}

    virtual ~AbstractFilter() = default;
};

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
        auto out = processImpl(std::move(in));

        return out;
    }

    typedef InData inType;
    typedef OutData outType;
};

/// Convenience type for shared_ptr to filters
template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

} // namespace blpl
