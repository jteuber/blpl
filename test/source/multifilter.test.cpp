#include <blpl/Filter.h>
#include <blpl/MultiFilter.h>

#include <doctest/doctest.h>

using namespace blpl;

// anonymous namespace to prevent clashes between test files
namespace {

class TestFilter1 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) / 2.f;
    }

    void reset() override
    {
        ++resetted;
    }

    int resetted = 0;
};

class TestFilter2 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) * 2.f;
    }

    void reset() override
    {
        ++resetted;
    }

    int resetted = 0;
};

TEST_CASE("simple multifilter construction")
{
    auto filter1     = std::make_shared<TestFilter1>();
    auto filter2     = std::make_shared<TestFilter2>();
    auto multifilter = filter1 & filter2;

    REQUIRE(multifilter.size() == 2);
}

TEST_CASE("bigger multifilter construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();
    auto filter3 = std::make_shared<TestFilter2>();

    auto multifilter = filter1 & filter2 & filter3;

    REQUIRE(multifilter.size() == 3);
}

TEST_CASE("multifilter construction from vector")
{
    std::vector<std::shared_ptr<TestFilter2>> vector;
    vector.push_back(std::make_shared<TestFilter2>());
    vector.push_back(std::make_shared<TestFilter2>());
    vector.push_back(std::make_shared<TestFilter2>());

    auto multifilter = MultiFilter<int, float>(vector);

    REQUIRE(multifilter.size() == 3);
}

TEST_CASE("simple multifilter process")
{
    auto filter1     = std::make_shared<TestFilter1>();
    auto filter2     = std::make_shared<TestFilter2>();
    auto multifilter = filter1 & filter2;

    std::vector<int> in(2, 2);
    std::vector<float> out = multifilter.process(std::move(in));

    REQUIRE(out[0] * out[1] == 4);
}

TEST_CASE("reset")
{
    auto filter1     = std::make_shared<TestFilter1>();
    auto filter2     = std::make_shared<TestFilter2>();
    auto multifilter = filter1 & filter2;

    REQUIRE(filter1->resetted == 0);
    REQUIRE(filter2->resetted == 0);

    multifilter.reset();

    REQUIRE(filter1->resetted == 1);
    REQUIRE(filter2->resetted == 1);

    multifilter.reset();

    REQUIRE(filter1->resetted == 2);
    REQUIRE(filter2->resetted == 2);
}

} // namespace
