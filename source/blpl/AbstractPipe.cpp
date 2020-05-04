#include "blpl/AbstractPipe.h"

namespace blpl {

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
