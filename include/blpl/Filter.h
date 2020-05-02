#pragma once

#include "Pipeline_global.h"

#include <memory>

#include "Profiler.h"

namespace blpl {

class PIPELINE_EXPORT AbstractFilter
{
public:
    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset(){}

    virtual ~AbstractFilter() = default;
};

/**
 * @brief This is the interface for all filters of the pipeline. It takes an
 * input, processes it and produces an output to be processed by the next
 * filter.
 */
template <class InData, class OutData>
class PIPELINE_EXPORT Filter : public AbstractFilter
{
protected:
    /**
     * @brief When implementing this interface, this is the part that's used in
     * the pipeline.
     *
     * @param in Shared pointer to some kind of input data of the type
     * PipeData.
     *
     * @return Shared pointer to the output of the type PipeData.
     */
    virtual OutData processImpl(InData&& in) = 0;

public:
    Filter()
        : m_prof(typeid(*this).name())
    {}

    virtual OutData process(InData&& in)
    {
        m_prof.startCycle();
        auto out = processImpl(std::move(in));
        m_prof.endCycle();

        return out;
    }

    typedef InData inType;
    typedef OutData outType;

private:
    Profiler m_prof;
};

/// Convenience type for shared_ptr to filters
template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

#define FilterCast(InData, OutData, ConcreteFilter)                            \
    std::static_pointer_cast<Filter<InData, OutData>>(ConcreteFilter)

} // namespace blpl
