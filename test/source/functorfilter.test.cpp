#include <blpl/FunctorFilter.h>

#include <doctest/doctest.h>

using namespace blpl;

TEST_CASE("construct and run")
{
    FunctorFilter<int, int> filter([](int&& in) -> int { return in + 1; });

    REQUIRE(filter.process(4) == 5);
}
