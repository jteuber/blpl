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
        if (m_listener)
            m_listener->preProcessCallback(in);
        auto out = processImpl(std::move(in));
        if (m_listener)
            m_listener->postProcessCallback(out);

        return out;
    }

    [[nodiscard]] bool isMultiFilter() const noexcept override
    {
        return false;
    }
    [[nodiscard]] size_t numParallel() const noexcept override
    {
        return 1;
    }

    typedef InData inType;
    typedef OutData outType;

    const std::type_info& getInDataTypeInfo() const noexcept override
    {
        return typeid(InData);
    }
    const std::type_info& getOutDataTypeInfo() const noexcept override
    {
        return typeid(OutData);
    }
};

/// Convenience type for shared_ptr to filters
template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

} // namespace blpl
