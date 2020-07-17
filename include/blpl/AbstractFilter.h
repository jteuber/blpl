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

    virtual ~AbstractFilterListener() = default;
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
    virtual void reset() {}

    /**
     * @brief Set a listener for this filter.
     */
    void setListener(
        const std::shared_ptr<AbstractFilterListener>& listener) noexcept
    {
        m_listener = listener;
    }

    /**
     * @brief Returns whether this is a MultiFilter aka has several parallel
     * filters.
     */
    [[nodiscard]] virtual bool isMultiFilter() const = 0;
    /**
     * @brief Returns the number of parallel filters if this is a MultiFilter.
     */
    [[nodiscard]] virtual size_t numParallel() const = 0;

    /**
     * @brief Returns the type_info for the type that is passed into this
     * filter.
     */
    [[nodiscard]] virtual const std::type_info& getInDataTypeInfo() const = 0;
    /**
     * @brief Returns the type_info for the type that is returned by the
     * process() method of this filter.
     */
    [[nodiscard]] virtual const std::type_info& getOutDataTypeInfo() const = 0;

    virtual ~AbstractFilter() = default;

protected:
    std::shared_ptr<AbstractFilterListener> m_listener = nullptr;
};

} // namespace blpl
