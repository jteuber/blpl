#include <blpl/Filter.h>
#include <blpl/InterceptingFilterListener.h>
#include <blpl/ProfilingFilterListener.h>

#include <doctest/doctest.h>

using namespace blpl;

class Passthrough : public Filter<int, int>
{
public:
    int processImpl(int&& in) override
    {
        return std::move(in);
    }
};

TEST_CASE("profiling")
{
    auto filter   = std::make_shared<Passthrough>();
    auto profiler = std::make_shared<ProfilingFilterListener>();
    filter->setListener(profiler);

    auto out = filter->process(1);
    REQUIRE(out == 1);
    REQUIRE(profiler->counter() == 1);
    auto walltime = profiler->wallTime().count();
    REQUIRE(walltime > 0);

    out = filter->process(2);
    REQUIRE(out == 2);
    REQUIRE(profiler->counter() == 2);
    REQUIRE(profiler->wallTime().count() > walltime);

    profiler->resetMetrics();
    REQUIRE(profiler->counter() == 0);
    REQUIRE(profiler->wallTime().count() == 0);

    out = filter->process(3);
    REQUIRE(out == 3);
    REQUIRE(profiler->counter() == 1);
    REQUIRE(profiler->wallTime().count() > 0);
}

TEST_CASE("intercepting")
{
    auto filter      = std::make_shared<Passthrough>();
    auto interceptor = std::make_shared<InterceptingFilterListener>();
    filter->setListener(interceptor);

    auto out = filter->process(1);
    REQUIRE(out == 1);
    interceptor->doOnLastOutData([out](const std::any& lastOut) {
        REQUIRE(std::any_cast<int>(lastOut) == out);
    });

    interceptor->doOnNextOutData([](const std::any& thisOut) {
        REQUIRE(std::any_cast<int>(thisOut) == 2);
    });
    out = filter->process(2);
    REQUIRE(out == 2);

    // making sure that doOnNextOutData is not called again
    out = filter->process(3);
    REQUIRE(out == 3);
}
