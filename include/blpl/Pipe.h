#pragma once

#include <thread>

#include "AbstractPipe.h"
#include "Generator.h"

namespace blpl {

/**
 * Implements the pipes in the pipeline.
 * @tparam TData Type of the data to pass through the Pipe.
 */
template <typename TData>
class Pipe : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false);
    virtual ~Pipe() = default;

    TData pop() noexcept;
    TData blockingPop() noexcept;

    void push(TData&& data) noexcept;

private:
    TData m_elem;
};

template <typename TData>
Pipe<TData>::Pipe(bool waitForSlowestFilter)
    : AbstractPipe(waitForSlowestFilter)
{}

template <typename TData>
TData Pipe<TData>::pop() noexcept
{
    TData temp = std::move(m_elem);
    m_valid    = false;
    return temp;
}

template <typename TData>
TData Pipe<TData>::blockingPop() noexcept
{
    while (!m_valid && m_enabled)
        std::this_thread::yield();

    return pop();
}

template <typename TData>
void Pipe<TData>::push(TData&& data) noexcept
{
    while (m_waitForSlowestFilter && m_valid && m_enabled)
        std::this_thread::yield();

    if (!m_enabled)
        return;

    m_valid = false;
    m_elem  = std::move(data);
    m_valid = true;
    m_pushCallback();
}

/// GENERATOR PIPES
template <>
class Pipe<Generator> : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false)
        : AbstractPipe(waitForSlowestFilter)
    {}
    virtual ~Pipe() = default;

    Generator pop() noexcept
    {
        return Generator();
    }
    Generator blockingPop() noexcept
    {
        return pop();
    }

    unsigned int size() const noexcept override
    {
        return 1;
    }
};
template <>
class Pipe<std::vector<Generator>> : public AbstractPipe
{
public:
    explicit Pipe(bool waitForSlowestFilter = false)
        : AbstractPipe(waitForSlowestFilter)
    {}
    virtual ~Pipe() = default;

    std::vector<Generator> pop() noexcept
    {
        return std::vector<Generator>();
    }
    std::vector<Generator> blockingPop() noexcept
    {
        return pop();
    }

    unsigned int size() const noexcept override
    {
        return 1;
    }
};

} // namespace blpl
