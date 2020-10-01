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

    void reset() noexcept
    {
        m_valid = false;
    }
    void disable() noexcept
    {
        m_enabled = false;
    }
    void enable() noexcept
    {
        m_enabled = true;
    }
    void setWaitForSlowestFilter(bool newValue) noexcept
    {
        m_waitForSlowestFilter = newValue;
    }

    virtual unsigned int size() const noexcept
    {
        return m_valid ? 1 : 0;
    }

    void registerPushCallback(std::function<void()> pushCallback) noexcept
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
