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

unsigned int AbstractPipe::size()
{
    return m_valid ? 1 : 0;
}

} // namespace blpl
