#pragma once

#include "Pipeline_global.h"

namespace blpl {

class PIPELINE_EXPORT AbstractFilterThread
{
public:
    virtual ~AbstractFilterThread() = default;
    virtual void start() = 0;
    virtual void stop()  = 0;
};

} // namespace blpl
