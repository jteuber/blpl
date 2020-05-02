#pragma once

#include "Pipeline_global.h"

#include <atomic>

namespace blpl {

class AbstractPipe
{
public:
    explicit AbstractPipe(bool waitForSlowestFilter = false);

    void reset();
    void disable();
    void enable();
    unsigned int size();

protected:
    std::atomic<bool> m_valid;
    bool m_waitForSlowestFilter;
    bool m_enabled;
};

}
