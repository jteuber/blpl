#pragma once

#include <cassert>
#include <thread>
#include <vector>

#include "Filter.h"

namespace blpl {

/**
 * @brief This is a specific filter that takes a number of filters and executes
 * them in lockstep on the input data which has to be supplied in a vector of at
 * least the number of filters.
 *
 * @note A multifilter should be constructed by stringing together filters with
 * the &-operator.
 */
template <class InData, class OutData>
class MultiFilter : public Filter<std::vector<InData>, std::vector<OutData>>
{
public:
    template <class Filter1, class Filter2>
    MultiFilter(std::shared_ptr<Filter1> first,
                std::shared_ptr<Filter2> second);

    template <class FilterClass>
    explicit MultiFilter(
        std::vector<std::shared_ptr<FilterClass>> filterVector);

    template <class ExtendingFilter>
    MultiFilter<InData, OutData>&
    operator&(std::shared_ptr<ExtendingFilter> filter);

    [[nodiscard]] bool isMultiFilter() const noexcept override
    {
        return true;
    }
    [[nodiscard]] size_t numParallel() const noexcept override
    {
        return m_filters.size();
    }

    void reset() noexcept override
    {
        for (auto& filter : m_filters)
            filter->reset();
    }

    const std::type_info& getInDataTypeInfo() const noexcept override
    {
        return typeid(InData);
    }
    const std::type_info& getOutDataTypeInfo() const noexcept override
    {
        return typeid(OutData);
    }

protected:
    std::vector<OutData> processImpl(std::vector<InData>&& in) override;

private:
    std::vector<FilterPtr<InData, OutData>> m_filters;
};

template <class InData, class OutData>
template <class Filter1, class Filter2>
MultiFilter<InData, OutData>::MultiFilter(std::shared_ptr<Filter1> first,
                                          std::shared_ptr<Filter2> second)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, Filter1>::value,
        "First filter does not inherit from the correct Filter template");
    static_assert(
        std::is_base_of<Filter<InData, OutData>, Filter2>::value,
        "Second filter does not inherit from the correct Filter template");

    m_filters.push_back(first);
    m_filters.push_back(second);
}

template <class InData, class OutData>
template <class FilterClass>
MultiFilter<InData, OutData>::MultiFilter(
    std::vector<std::shared_ptr<FilterClass>> filterVector)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, FilterClass>::value,
        "Given filters do not inherit from the correct Filter template");

    // just copy the vector
    for (auto& filter : filterVector) {
        m_filters.push_back(filter);
    }
}

template <class InData, class OutData>
template <class ExtendingFilter>
MultiFilter<InData, OutData>&
MultiFilter<InData, OutData>::operator&(std::shared_ptr<ExtendingFilter> filter)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, ExtendingFilter>::value,
        "Extending filter does not inherit from the correct Filter template");

    m_filters.push_back(filter);

    return *this;
}

template <class Filter1, class Filter2>
MultiFilter<typename Filter1::inType, typename Filter1::outType>
operator&(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second)
{
    return MultiFilter<typename Filter1::inType, typename Filter1::outType>(
        first, second);
}

template <class InData, class OutData>
std::vector<OutData>
MultiFilter<InData, OutData>::processImpl(std::vector<InData>&& in)
{
    assert(!m_filters.empty());
    std::vector<OutData> out(m_filters.size());

    if (in.size() >= m_filters.size()) {
        // call process of all sub-filters in their own thread
        std::vector<std::thread> threads;
        for (size_t i = 1; i < m_filters.size(); ++i)
            threads.emplace_back([&out    = out[i],
                                  &filter = m_filters[i],
                                  in      = std::move(in[i])]() mutable {
                out = filter->process(std::move(in));
            });

        // but execute the first filter in this thread
        out[0] = m_filters[0]->process(std::move(in[0]));

        for (auto& thread : threads)
            thread.join();
    }

    return out;
}

} // namespace blpl
