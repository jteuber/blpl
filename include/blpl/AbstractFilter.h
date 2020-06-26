#pragma once

#include <any>
#include <chrono>
#include <cstdint>
#include <memory>

namespace blpl {

/**
 * @brief Provides a way to inject introspecting code into a Filter without
 * changing the actual filter.
 */
class AbstractFilterListener
{
public:
    virtual void preProcessCallback(const std::any& in)   = 0;
    virtual void postProcessCallback(const std::any& out) = 0;
};

/**
 * @brief Abstract superclass for the filter with methods that are not dependend
 * on the template parameters.
 */
class AbstractFilter
{
public:
    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset(){};

    virtual ~AbstractFilter() = default;

    void setListener(const std::shared_ptr<AbstractFilterListener>& listener)
    {
        m_listener = listener;
    }

protected:
    std::shared_ptr<AbstractFilterListener> m_listener = nullptr;
};

} // namespace blpl
