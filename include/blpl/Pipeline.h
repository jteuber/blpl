#pragma once

#include <list>

#include "AbstractPipeline.h"
#include "FilterThread.h"

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
class Pipeline : public AbstractPipeline
{
public:
    // pipe operators for waiting pipes
    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator|(std::shared_ptr<ExtendingFilter> filter);

    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator|(ExtendingFilter&& filter);

    // pipe operators for discarding pipes
    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator>(std::shared_ptr<ExtendingFilter> filter);

    template <class ExtendingFilter>
    Pipeline<InData, typename ExtendingFilter::outType>
    operator>(ExtendingFilter&& filter);

    // constructors
    template <class Filter1, class Filter2>
    Pipeline(std::shared_ptr<Filter1> first,
             std::shared_ptr<Filter2> second,
             bool waitForSlowestFilter = true);

    template <class ExtendingFilter>
    Pipeline(Pipeline<InData, typename ExtendingFilter::inType>&& pipeline,
             std::shared_ptr<ExtendingFilter> extender,
             bool waitForSlowestFilter = true);

    std::shared_ptr<Pipe<OutData>> outPipe()
    {
        return m_outPipe;
    }

    std::shared_ptr<Pipe<InData>> inPipe()
    {
        return m_inPipe;
    }

private:
    explicit Pipeline() = default;

private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Pipe<OutData>> m_outPipe;

    template <class, class>
    friend class Pipeline;
};

/// ===========================================================================
///                              WAITING PIPES
/// ===========================================================================

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
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
    return Pipeline<InData, typename ExtendingFilter::outType>(
        std::move(*this), filter, true);
}

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
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
    return Pipeline<InData, typename ExtendingFilter::outType>(
        std::move(*this), ptr, true);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second)
{
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        first, second, true);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(Filter1&& first, Filter2&& second)
{
    std::shared_ptr<Filter1> ptr1(new Filter1(first));
    std::shared_ptr<Filter2> ptr2(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        ptr1, ptr2, true);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(std::shared_ptr<Filter1> first, Filter2&& second)
{
    std::shared_ptr<Filter2> ptr(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        first, ptr, true);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a waiting pipe, meaning the
 * previous filter has to wait to push a new value until the following filter
 * hasn't pops the value currently in the pipe.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator|(Filter1&& first, std::shared_ptr<Filter2> second)
{
    std::shared_ptr<Filter1> ptr(new Filter1(first));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        ptr, second, true);
}

/// ===========================================================================
///                            DISCARDING PIPES
/// ===========================================================================

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
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
Pipeline<InData, OutData>::operator>(std::shared_ptr<ExtendingFilter> filter)
{
    return Pipeline<InData, typename ExtendingFilter::outType>(
        std::move(*this), filter, false);
}

/**
 * @brief Pipe operator to string together filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
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
Pipeline<InData, OutData>::operator>(ExtendingFilter&& filter)
{
    std::shared_ptr<ExtendingFilter> ptr(new ExtendingFilter(filter));
    return Pipeline<InData, typename ExtendingFilter::outType>(
        std::move(*this), ptr, false);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator>(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second)
{
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        first, second, false);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator>(Filter1&& first, Filter2&& second)
{
    std::shared_ptr<Filter1> ptr1(new Filter1(first));
    std::shared_ptr<Filter2> ptr2(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        ptr1, ptr2, false);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
 *
 * @param first The first filter of the new pipeline in a shared pointer
 * @param second The second and last filter of the new pipeline moved into the
 * pipeline
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator>(std::shared_ptr<Filter1> first, Filter2&& second)
{
    std::shared_ptr<Filter2> ptr(new Filter2(second));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        first, ptr, false);
}

/**
 * @brief Pipe operator stringing together two filters to form a pipeline.
 *
 * @note The pipe represented by this operator is a discarding pipe, meaning the
 * previous filter will overwrite the value in the pipe if the following filter
 * hasn't popped it yet.
 *
 * @param first The first filter of the new pipeline moved into the pipeline
 * @param second The second and last filter of the new pipeline in a shared
 * pointer
 *
 * @return A pipeline with the two filters stringed together.
 */
template <class Filter1, class Filter2>
Pipeline<typename Filter1::inType, typename Filter2::outType>
operator>(Filter1&& first, std::shared_ptr<Filter2> second)
{
    std::shared_ptr<Filter1> ptr(new Filter1(first));
    return Pipeline<typename Filter1::inType, typename Filter2::outType>(
        ptr, second, false);
}

/// ===========================================================================
///                               CONSTRUCTORS
/// ===========================================================================

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
    std::shared_ptr<ExtendingFilter> extender,
    bool waitForSlowestFilter)
    : m_inPipe(std::move(pipeline.m_inPipe))
{
    static_assert(
        std::is_same<typename ExtendingFilter::outType, OutData>::value,
        "Filters are incompatible");

    m_filterThreads = std::move(pipeline.m_filterThreads);
    m_filters       = std::move(pipeline.m_filters);

    // prepare the pipe
    auto betweenPipe = std::move(pipeline.m_outPipe);
    betweenPipe->setWaitForSlowestFilter(waitForSlowestFilter);
    m_outPipe = std::make_shared<Pipe<OutData>>(false);

    // add the filter thread
    m_filterThreads.push_back(
        std::make_shared<
            FilterThread<typename ExtendingFilter::inType, OutData>>(
            betweenPipe, extender, m_outPipe));

    m_filters.push_back(extender);
}

/**
 * @brief Construct a new Pipeline object from two filters.
 *
 * @note This is not meant for use in consumer code, use operator| instead.
 */
template <class InData, class OutData>
template <class Filter1, class Filter2>
Pipeline<InData, OutData>::Pipeline(std::shared_ptr<Filter1> first,
                                    std::shared_ptr<Filter2> second,
                                    bool waitForSlowestFilter)
{
    static_assert(std::is_same<typename Filter1::outType,
                               typename Filter2::inType>::value,
                  "Filters are incompatible");
    static_assert(std::is_same<typename Filter1::inType, InData>::value,
                  "Filters are incompatible");
    static_assert(std::is_same<typename Filter2::outType, OutData>::value,
                  "Filters are incompatible");

    // start with the pipes
    m_inPipe = std::make_shared<Pipe<typename Filter1::inType>>(false);
    auto betweenPipe =
        std::make_shared<Pipe<typename Filter1::outType>>(waitForSlowestFilter);
    m_outPipe = std::make_shared<Pipe<typename Filter2::outType>>(false);

    // then create the filter threads
    m_filterThreads.push_back(
        std::make_shared<
            FilterThread<typename Filter1::inType, typename Filter1::outType>>(
            m_inPipe, first, betweenPipe));
    m_filterThreads.push_back(
        std::make_shared<
            FilterThread<typename Filter2::inType, typename Filter2::outType>>(
            betweenPipe, second, m_outPipe));

    m_filters.push_back(first);
    m_filters.push_back(second);
}

} // namespace blpl
