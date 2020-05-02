#pragma once

#include <functional>

#include "Filter.h"

namespace blpl {

template <class InData, class OutData>
class FunctorFilter : public Filter<InData, OutData>
{
public:
    FunctorFilter(std::function<OutData(InData&&)> filterFunc);

protected:
    OutData processImpl(InData&& in) override;

private:
    std::function<OutData(InData)> m_filterFunc;
};

template <class InData, class OutData>
FunctorFilter<InData, OutData>::FunctorFilter(
    std::function<OutData(InData&&)> filterFunc)
    : m_filterFunc(filterFunc)
{}

template <class InData, class OutData>
OutData FunctorFilter<InData, OutData>::processImpl(InData&& in)
{
    return m_filterFunc(in);
}

} // namespace blpl
