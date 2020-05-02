#include <blpl/FilterThread.h>

#include <doctest/doctest.h>

using namespace blpl;

class Passthrough : public Filter<int, int>
{
public:
    int processImpl(int&& in) override
    {
        return std::move(in);
    }
    void reset() override {}
};

TEST_CASE("construction")
{
    auto inPipe  = std::make_shared<Pipe<int>>();
    auto outPipe = std::make_shared<Pipe<int>>();
    auto filter  = std::make_shared<Passthrough>();
    FilterThread<int, int> ft(inPipe, filter, outPipe);

    REQUIRE_FALSE(ft.isFiltering());
}

TEST_CASE("start")
{
    auto inPipe  = std::make_shared<Pipe<int>>();
    auto outPipe = std::make_shared<Pipe<int>>();
    auto filter  = std::make_shared<Passthrough>();
    FilterThread<int, int> ft(inPipe, filter, outPipe);

    ft.start();
    inPipe->push(1);
    REQUIRE(outPipe->blockingPop() == 1);
    REQUIRE(ft.isFiltering());

    inPipe->push(2);
    REQUIRE(outPipe->blockingPop() == 2);
}

TEST_CASE("stop")
{
    auto inPipe  = std::make_shared<Pipe<int>>();
    auto outPipe = std::make_shared<Pipe<int>>();
    auto filter  = std::make_shared<Passthrough>();
    FilterThread<int, int> ft(inPipe, filter, outPipe);

    ft.start();
    inPipe->push(1);
    REQUIRE(outPipe->blockingPop() == 1);

    REQUIRE(ft.isFiltering());
    ft.stop();
    REQUIRE_FALSE(ft.isFiltering());

    inPipe->push(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(outPipe->size() == 0);
}
