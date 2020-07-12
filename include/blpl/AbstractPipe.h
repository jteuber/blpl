#pragma once

#include <atomic>
#include <functional>

namespace blpl {

class AbstractPipe
{
public:
    explicit AbstractPipe(bool waitForSlowestFilter = false)
        : m_valid(false)
        , m_waitForSlowestFilter(waitForSlowestFilter)
        , m_enabled(true)
    {}

    void reset()
    {
        m_valid = false;
    }
    void disable()
    {
        m_enabled = false;
    }
    void enable()
    {
        m_enabled = true;
    }
    void setWaitForSlowestFilter(bool newValue)
    {
        m_waitForSlowestFilter = newValue;
    }

    virtual unsigned int size() const
    {
        return m_valid ? 1 : 0;
    }

    void registerPushCallback(std::function<void()> pushCallback)
    {
        m_pushCallback = pushCallback;
    }

protected:
    std::atomic<bool> m_valid;
    bool m_waitForSlowestFilter;
    bool m_enabled;

    std::function<void()> m_pushCallback = [] {};
};

} // namespace blpl
