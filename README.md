[![Actions Status](https://github.com/jteuber/blpl/workflows/MacOS/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Windows/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Ubuntu/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Style/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Install/badge.svg)](https://github.com/jteuber/blpl/actions)
[![codecov](https://codecov.io/gh/jteuber/blpl/branch/master/graph/badge.svg)](https://codecov.io/gh/jteuber/blpl)

# blpl

Welcome to the big, lightweight pipeline: a modern C++ pipeline library for high concurrency, high through-put
computation on the CPU.

The blpl was developed to be a very easy to use and lightweight pipeline library that enables very high concurrency
 on pipelines with a medium to high number of filters. It supports parallel filters that run in lock step (see MultiFilter)
  and filters that split data to input into or join the output of a MultiFilter as simple std::vectors.

The concurrency is happening on a per filter-instance level. This means that one specific filter in the pipeline is
 always running sequentially and never parallel. This enables the filter to have state, while all filters run parallel
  to each other.

## How to compile

This library is using cmake to build and install. Just do
```shell script
mkdir build && cd build && cmake .. && make && sudo make install
```
in the library directory.

The library has for now only been tested on linux. It might still have some hickups on windows and mac, but should in theory (tm) work fine.

## How to use

The following is a very simple example program.

```c++
#include <blpl/Pipeline.h>

// write a filter by inheriting Filter<InData,OutData>. *Generator* is a special type that tells the pipeline to
// continuously call this filter.
class StartFilter : public blpl::Filter<Generator, int>
{
public:
    // Implement your filter in processImpl
    int processImpl(Generator&&) override
    {
        return m_i++;
    }

    // and a reset method to reset any state of the filter to its initial state
    void reset() override { m_i = 0; }
    // we don't have to worry about the state being thread safe as every instance of this filter will run sequentially
    int m_i = 0;
};

// and then a few more that do actual computation
...
// and a last one that produces and maybe stores or shows the final output
class EndFilter : public blpl::Filter<std::string, int>
{
public:
    // the payload in the pipes are moved to have as little overhead as possible. If you need shared resources just
    // use shared pointers to those resources.
    int processImpl(std::string&& in) override
    {
        std::cout << in;
        return 0;
    }
};

int main()
{
    // instantiate the filters
    auto startFilter = std::make_shared<StartFilter>();
    auto filter1 = std::make_shared<TestFilter1>();
    ...
    auto endFilter = std::make_shared<EndFilter>();

    // build the pipeline
    auto pipeline = startFilter |
                    filter1 |
                    ... |
                    endFilter;

    // start the pipeline
    pipeline.start();

    // this is one way of waiting for the pipeline to be done, just return a status code in the last filter
    int status = 0;
    while(status == 0)
        status = pipeline.outPipe()->blockingPop();

    // stop the pipeline. Stops the pipes and waits for all the threads to finish up their last computation
    pipeline.stop();
}
```


## Changelog

### v0.2.1

* Fix a possible race condition
* More documentation, method specifiers and code cleanup

### v0.2.0

* Pipes now support callbacks on push
* FilterThreads use these callbacks instead of busy waiting to start processing when waiting
for the preceding filter

### v0.1.6 (hotfix release)

* Fix failing tests
* Initialize members in ProfilingFilterListener

### v0.1.5

* Fix test on Windows
* Fix a bug in the pipe that could lead to crashes
* Add functionality to add a listener to filters
  * Provide a profiling and an intercepting filter listener out of the box
* Add API to get the type_info of the InData and OutData of a Filter in AbstractFilter

### v0.1.4

* Made library header-only
* Removed log and profiler classes
* Added reset method to pipeline that resets all filters and restarts their threads if running

### v0.1.3

* Fix compiling on windows with threading library

### v0.1.2

* Remove defines for dll-linkage, now static lib only
* Add new pipe operator ">" that creates discarding pipes
* Make existing pipe operator "|" create waiting pipes
* Make it possible to construct filters directly into the pipeline

### v0.1.1

* Add MultiFilter, a class that encapsulates several filters into one, making them run in lock-step
* Use namespace "blpl" everywhere

### v0.1.0

* initial version with single-flow pipeline as shared object/dll


## License

This library uses the MIT license.
