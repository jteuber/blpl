#pragma once

#include <atomic>

namespace blpl {

class AbstractPipe
{
public:
    explicit AbstractPipe(bool waitForSlowestFilter = false);

    void reset();
    void disable();
    void enable();
    void setWaitForSlowestFilter(bool newValue);

    unsigned int size() const;

protected:
    std::atomic<bool> m_valid;
    bool m_waitForSlowestFilter;
    bool m_enabled;
};

AbstractPipe::AbstractPipe(bool waitForSlowestFilter)
    : m_valid(false)
    , m_waitForSlowestFilter(waitForSlowestFilter)
    , m_enabled(true)
{}

void AbstractPipe::reset()
{
    m_valid = false;
}

void AbstractPipe::disable()
{
    m_enabled = false;
}

void AbstractPipe::enable()
{
    m_enabled = true;
}

void AbstractPipe::setWaitForSlowestFilter(bool newValue)
{
    m_waitForSlowestFilter = newValue;
}

unsigned int AbstractPipe::size() const
{
    return m_valid ? 1 : 0;
}

} // namespace blpl
