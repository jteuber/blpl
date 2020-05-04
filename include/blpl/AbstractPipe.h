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

} // namespace blpl
