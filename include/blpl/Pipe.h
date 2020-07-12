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
    TData temp = std::move(m_elem);
    m_valid    = false;
    return temp;
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

    Generator pop()
    {
        return Generator();
    }
    Generator blockingPop()
    {
        return pop();
    }

    unsigned int size() const override
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

    std::vector<Generator> pop()
    {
        return std::vector<Generator>();
    }
    std::vector<Generator> blockingPop()
    {
        return pop();
    }

    unsigned int size() const override
    {
        return 1;
    }
};

} // namespace blpl
