#pragma once

#include <memory>

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
        resetCounter();
    }

    virtual ~AbstractFilter() = default;

    /**
     * @brief Returns the number of runs of the filter since the start of the
     * pipeline or last reset.
     */
    uint32_t counter()
    {
        return m_counter;
    }
    void resetCounter()
    {
        m_counter = 0;
    }

protected:
    uint32_t m_counter = 0;
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
