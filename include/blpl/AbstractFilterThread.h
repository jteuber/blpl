#pragma once

namespace blpl {

class AbstractFilterThread
{
public:
    virtual ~AbstractFilterThread() = default;
    virtual void start()            = 0;
    virtual void stop()             = 0;
    virtual void reset()            = 0;
};

} // namespace blpl
