[![Actions Status](https://github.com/jteuber/blpl/workflows/MacOS/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Windows/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Ubuntu/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Style/badge.svg)](https://github.com/jteuber/blpl/actions)
[![Actions Status](https://github.com/jteuber/blpl/workflows/Install/badge.svg)](https://github.com/jteuber/blpl/actions)
[![codecov](https://codecov.io/gh/jteuber/blpl/branch/master/graph/badge.svg)](https://codecov.io/gh/jteuber/blpl)

# blpl

Welcome to the big, lightweight pipeline: a modern C++ pipeline library for high concurrency, high through-put 
computation on the CPU.

WARNING: The API of this library is still very unstable in the 0.1.x times. I might change a lot from one subversion to the next. Only the what's used in *How to compile* is guarranteed to work in this minor version. (But it might change slightly in 0.2.)

The concurrency is happening on a per filter-instance level. This means that one specific filter in the pipeline is
 always running sequentially, making it possible for the filter to have state, while all filters run parallel to each
  other, enabling very high concurrency on pipelines with a medium to high number of filters.

This library was originally developed for a research project using computer-vision and machine-learning. The goal was
 to have a lightweight pipeline library that adds as little overhead as possible. 

## How to compile

This library is using cmake to build and install. Just do
```shell script
mkdir build && cd build && cmake .. && make && sudo make install
```
in the library directory.

The library has for now only been tested on linux. It might still have some hickups on windows and mac, but should in theory (tm) work fine.
 
## How to use

```c++
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


## License

This library uses the MIT license.
