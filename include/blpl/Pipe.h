#pragma once

#include "Pipeline_global.h"

#include <thread>

#include "AbstractPipe.h"
#include "Generator.h"

namespace blpl {

/**
 * Implements the pipes in the pipeline.
 * @tparam TData Type of the data to pass through the Pipe.
 */
template <typename TData>
class PIPELINE_EXPORT Pipe : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false);
    virtual ~Pipe() = default;

    TData pop();
    TData blockingPop();

    void push(TData&& data);

private:
    TData m_elem;
};

template <typename TData>
Pipe<TData>::Pipe(bool waitForSlowestFilter)
    : AbstractPipe(waitForSlowestFilter)
{}

template <typename TData>
TData Pipe<TData>::pop()
{
    m_valid = false;
    return std::move(m_elem);
}

template <typename TData>
TData Pipe<TData>::blockingPop()
{
    while (!m_valid && m_enabled)
        std::this_thread::yield();

    return pop();
}

template <typename TData>
void Pipe<TData>::push(TData&& data)
{
    while (m_waitForSlowestFilter && m_valid && m_enabled)
        std::this_thread::yield();

    if (!m_enabled)
        return;

    m_elem  = std::move(data);
    m_valid = true;
}

/// GENERATOR PIPES
template <>
class PIPELINE_EXPORT Pipe<Generator> : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false)
        : AbstractPipe(waitForSlowestFilter)
    {}
    virtual ~Pipe() = default;

    Generator pop()
    {
        return Generator();
    }
    Generator blockingPop()
    {
        return pop();
    }
};
template <>
class PIPELINE_EXPORT Pipe<std::vector<Generator>> : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false)
        : AbstractPipe(waitForSlowestFilter)
    {}
    virtual ~Pipe() = default;

    std::vector<Generator> pop()
    {
        return std::vector<Generator>();
    }
    std::vector<Generator> blockingPop()
    {
        return pop();
    }
};

} // namespace blpl
