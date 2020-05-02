#pragma once

#include "Pipeline_global.h"

#include <list>

#include "AbstractPipeline.h"
#include "FilterThread.h"
#include "Pipe.h"

namespace blpl {

/**
 * Main pipeline class.
 * @tparam InData The datatype that is consumed by the pipeline.
 * @tparam OutData The datatype that is produced by the pipeline.
 *
 * @note When information about the input and output datatype are not needed,
 * consider storing the pipeline as an AbstractPipeline instead.
 */
template <class InData, class OutData>
class PIPELINE_EXPORT Pipeline : public AbstractPipeline
{
public:
    template <class Filter1, class Filter2>
    Pipeline(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second);

    template <class ExtendingFilter>
    Pipeline(Pipeline<InData, typename ExtendingFilter::inType>&& pipeline,
             std::shared_ptr<ExtendingFilter> extender);

    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator|(std::shared_ptr<ExtendingFilter> filter);

    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator|(ExtendingFilter&& filter);

    std::shared_ptr<Pipe<OutData>> outPipe()
    {
        return m_outPipe;
    }

    explicit Pipeline(bool waitForSlowestFilter) 
        : m_waitForSlowestFilter(waitForSlowestFilter)
    {}

private:
    explicit Pipeline() = default;

private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Pipe<OutData>> m_outPipe;

    bool m_waitForSlowestFilter = false;

    template <class, class>
    friend class Pipeline;
};

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @tparam ExtendingFilter The type of the filter with which the pipeline gets
 * extended.
 *
 * @param filter The filter that will be added to the end of the pipeline as
 * shared pointer
 * @return A new pipeline constructed out of this pipeline with the given filter
 * appended. The original pipeline will be invalidated.
 */
template <class InData, class OutData>
template <class ExtendingFilter>
Pipeline<InData, typename ExtendingFilter::outType>
Pipeline<InData, OutData>::operator|(std::shared_ptr<ExtendingFilter> filter)
{
    return Pipeline<InData, typename ExtendingFilter::outType>(std::move(*this),
                                                               filter);
}

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @tparam ExtendingFilter The type of the filter with which the pipeline gets
 * extended.
 *
 * @param filter The filter that will be added to the end of the pipeline
 * @return A new pipeline constructed out of this pipeline with the given filter
 * appended. The original pipeline will be invalidated.
 */
template <class InData, class OutData>
template <class ExtendingFilter>
Pipeline<InData, typename ExtendingFilter::outType>
Pipeline<InData, OutData>::operator|(ExtendingFilter&& filter)
{
    std::shared_ptr<ExtendingFilter> ptr(new ExtendingFilter(filter));
    return Pipeline<InData, typename ExtendingFilter::outType>(std::move(*this),
                                                               ptr);
}

/**
 * @brief Construct a new Pipeline object from another one and an extending
 * filter.
 *
 * @note This is not meant for use in consumer code, use operator| instead.
 */
template <class InData, class OutData>
template <class ExtendingFilter>
Pipeline<InData, OutData>::Pipeline(
    Pipeline<InData, typename ExtendingFilter::inType>&& pipeline,
    std::shared_ptr<ExtendingFilter> extender)
    : m_inPipe(std::move(pipeline.m_inPipe))
    , m_waitForSlowestFilter(std::move(pipeline.m_waitForSlowestFilter))
{
    static_assert(std::is_same<typename ExtendingFilter::outType,
                               OutData>::value,
                  "Filters are incompatible");

    m_filters = std::move(pipeline.m_filters);

    // prepare the pipe
    auto betweenPipe = std::move(pipeline.m_outPipe);
    m_outPipe = std::make_shared<Pipe<OutData>>(pipeline.m_waitForSlowestFilter);

    // add the filter thread
    m_filters.push_back(
        std::make_shared<
            FilterThread<typename ExtendingFilter::inType, OutData>>(
            betweenPipe, extender, m_outPipe));
}

/**
 * @brief Construct a new Pipeline object from two filters.
 *
 * @note This is not meant for use in consumer code, use operator| instead.
 */
template <class InData, class OutData>
template <class Filter1, class Filter2>
Pipeline<InData, OutData>::Pipeline(std::shared_ptr<Filter1> first,
                                    std::shared_ptr<Filter2> second)
{
    static_assert(std::is_same<typename Filter1::outType,
                               typename Filter2::inType>::value,
                  "Filters are incompatible");
    static_assert(std::is_same<typename Filter1::inType, InData>::value,
                  "Filters are incompatible");
    static_assert(std::is_same<typename Filter2::outType, OutData>::value,
                  "Filters are incompatible");

    // start with the pipes
    m_inPipe         = std::make_shared<Pipe<typename Filter1::inType>>(m_waitForSlowestFilter);
    auto betweenPipe = std::make_shared<Pipe<typename Filter1::outType>>(m_waitForSlowestFilter);
    m_outPipe        = std::make_shared<Pipe<typename Filter2::outType>>(m_waitForSlowestFilter);

    // then create the filter threads
    m_filters.push_back(
        std::make_shared<
            FilterThread<typename Filter1::inType, typename Filter1::outType>>(
            m_inPipe, first, betweenPipe));
    m_filters.push_back(
        std::make_shared<
            FilterThread<typename Filter2::inType, typename Filter2::outType>>(
            betweenPipe, second, m_outPipe));
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
PIPELINE_EXPORT Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second)
{
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        first, second);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
PIPELINE_EXPORT Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(Filter1&& first, Filter2&& second)
{
    std::shared_ptr<Filter1> ptr1(new Filter1(first));
    std::shared_ptr<Filter2> ptr2(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(ptr1,
                                                                         ptr2);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
PIPELINE_EXPORT Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(std::shared_ptr<Filter1> first, Filter2&& second)
{
    std::shared_ptr<Filter2> ptr(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(first,
                                                                         ptr);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
PIPELINE_EXPORT Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(Filter1&& first, std::shared_ptr<Filter2> second)
{
    std::shared_ptr<Filter1> ptr(new Filter1(first));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        ptr, second);
}

} // namespace blpl
