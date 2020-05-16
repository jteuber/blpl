#include "blpl/MultiFilter.h"
#include "blpl/Pipeline.h"

#include <doctest/doctest.h>

using namespace blpl;

// anonymous namespace to prevent clashes between test files
namespace {

class TestFilter0 : public Filter<Generator, int>
{
public:
    int processImpl(Generator&&) override
    {
        if (m_i < 100)
            return m_i++;
        return m_i;
    }

    void reset() override
    {
        Filter<Generator, int>::reset();
        m_i              = 0;
        m_resetWasCalled = true;
    }

    int m_i               = 0;
    bool m_resetWasCalled = false;
};

class TestFilter1 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) / 2.f;
    }
};

class TestFilter2 : public Filter<float, std::string>
{
public:
    std::string processImpl(float&& in) override
    {
        return std::to_string(in);
    }
};

class TestFilter3 : public Filter<std::string, std::string>
{
public:
    std::string processImpl(std::string&& in) override
    {
        m_lastInput = in;
        return std::move(in);
    }

    std::string m_lastInput;
};

class TestFilterMultiIn : public Filter<std::vector<float>, float>
{
public:
    float processImpl(std::vector<float>&& in) override
    {
        float ret = 0.f;
        for (float& f : in)
            ret += f;
        return ret / 2.f;
    }
};

TEST_CASE("simple pipeline construction")
{
    auto filter1  = FilterPtr<int, float>(new TestFilter1);
    auto filter2  = FilterPtr<float, std::string>(new TestFilter2);
    auto pipeline = filter1 | filter2;

    REQUIRE(pipeline.length() == 2);
}

TEST_CASE("bigger pipeline construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();
    auto filter3 = std::make_shared<TestFilter3>();

    auto pipeline = filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 3);
}

TEST_CASE("construction with rvalues")
{
    auto filter2 = std::make_shared<TestFilter2>();

    auto pipeline = TestFilter1() | filter2 | TestFilter3();

    REQUIRE(pipeline.length() == 3);
}

TEST_CASE("pipeline test with generator")
{
    auto filter0  = std::make_shared<TestFilter0>();
    auto filter1  = std::make_shared<TestFilter1>();
    auto filter2  = std::make_shared<TestFilter2>();
    auto filter3  = std::make_shared<TestFilter3>();
    auto pipeline = filter0 | filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 4);

    pipeline.start();
    for (int i = 0; i < 101; ++i) {
        pipeline.outPipe()->blockingPop();
    }
    std::this_thread::yield();
    pipeline.stop();

    CHECK(filter0->m_i == 100);
    REQUIRE(!filter3->m_lastInput.empty());
    CHECK(std::stoi(filter3->m_lastInput) == 50);
}

TEST_CASE("pipeline with rvalue filters")
{
    auto pipeline = TestFilter1() | TestFilter2() | TestFilter3();

    REQUIRE(pipeline.length() == 3);

    pipeline.start();
    std::string lastOut;
    for (int i = 0; i < 101; ++i) {
        int pipeData = i;
        pipeline.inPipe()->push(std::move(pipeData));
        lastOut = pipeline.outPipe()->blockingPop();
    }
    pipeline.stop();

    REQUIRE(!lastOut.empty());
    CHECK(std::stoi(lastOut) == 50);
}

TEST_CASE("pipeline with discarding filters")
{
    auto pipeline = TestFilter1() > TestFilter2() > TestFilter3();

    REQUIRE(pipeline.length() == 3);

    pipeline.start();
    for (int i = 0; i < 101; ++i) {
        int pipeData = i;
        pipeline.inPipe()->push(std::move(pipeData));
    }
    // wait for a bit to make sure the data went through the pipeline
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::string lastOut = pipeline.outPipe()->blockingPop();
    pipeline.stop();

    REQUIRE(!lastOut.empty());
    REQUIRE(std::stoi(lastOut) == 50);
}

TEST_CASE("pipeline with multifilter start")
{
    auto filter0_0 = std::make_shared<TestFilter1>();
    auto filter0_1 = std::make_shared<TestFilter1>();
    auto filter1   = std::make_shared<TestFilterMultiIn>();
    auto filter2   = std::make_shared<TestFilter2>();
    auto filter3   = std::make_shared<TestFilter3>();
    auto pipeline  = (filter0_0 & filter0_1) | filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 4);

    pipeline.start();
    std::string lastOut;
    for (int i = 0; i < 101; ++i) {
        std::vector<int> pipeData(2, i);
        pipeline.inPipe()->push(std::move(pipeData));
        lastOut = pipeline.outPipe()->blockingPop();
    }
    pipeline.stop();

    REQUIRE(!lastOut.empty());
    CHECK(std::stoi(lastOut) == 50);
}

TEST_CASE("pipeline with multifilter start to end")
{
    auto filter1_0 = std::make_shared<TestFilter1>();
    auto filter1_1 = std::make_shared<TestFilter1>();
    auto filter2_0 = std::make_shared<TestFilter2>();
    auto filter2_1 = std::make_shared<TestFilter2>();
    auto filter3_0 = std::make_shared<TestFilter3>();
    auto filter3_1 = std::make_shared<TestFilter3>();
    auto pipeline  = (filter1_0 & filter1_1) | (filter2_0 & filter2_1) |
                    (filter3_0 & filter3_1);

    REQUIRE(pipeline.length() == 3);

    pipeline.start();
    std::vector<std::string> lastOut;
    for (int i = 0; i < 101; ++i) {
        std::vector<int> pipeData(2, i);
        pipeline.inPipe()->push(std::move(pipeData));
        lastOut = pipeline.outPipe()->blockingPop();
    }
    pipeline.stop();

    REQUIRE(lastOut.size() == 2);
    REQUIRE(!lastOut[0].empty());
    CHECK(std::stoi(lastOut[0]) == 50);
    REQUIRE(!lastOut[1].empty());
    CHECK(std::stoi(lastOut[1]) == 50);
}

TEST_CASE("pipeline reset while running")
{
    auto filter0  = std::make_shared<TestFilter0>();
    auto filter1  = std::make_shared<TestFilter1>();
    auto filter2  = std::make_shared<TestFilter2>();
    auto filter3  = std::make_shared<TestFilter3>();
    auto pipeline = filter0 | filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 4);

    pipeline.start();
    for (int i = 0; i < 101; ++i) {
        pipeline.outPipe()->blockingPop();
    }

    REQUIRE(filter0->m_i > 0);

    pipeline.reset();

    REQUIRE(filter0->m_resetWasCalled);

    for (int i = 0; i < 101; ++i) {
        pipeline.outPipe()->blockingPop();
    }

    REQUIRE(filter0->m_i > 0);

    pipeline.stop();
}

TEST_CASE("pipeline reset after stop")
{
    auto filter0  = std::make_shared<TestFilter0>();
    auto pipeline = filter0 > TestFilter1() > TestFilter2() > TestFilter3();

    REQUIRE(pipeline.length() == 4);

    pipeline.start();
    for (int i = 0; i < 101; ++i) {
        pipeline.outPipe()->blockingPop();
    }
    std::this_thread::yield();
    pipeline.stop();

    REQUIRE(filter0->m_i > 0);

    pipeline.reset();

    REQUIRE(filter0->m_i == 0);

    // wait for a bit to make sure that no filterthread was actually started up
    // again
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(filter0->m_i == 0);
}

} // namespace
