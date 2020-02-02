
Reactor
=======
[![pipeline status](https://gitlab.iwstudio.hu/u-foka/reactor/badges/master/pipeline.svg)](https://gitlab.iwstudio.hu/u-foka/reactor/commits/master)
[![coverage report](https://gitlab.iwstudio.hu/u-foka/reactor/badges/master/coverage.svg)](https://gitlab.iwstudio.hu/u-foka/reactor/commits/master)

Reactor is a service locator implementation intended to replace traditional dependency injection techniques.

To achieve the above goal it binds factories to interface classes and when an interface is requested it either returns
the already produced "service" or creates it if necessary. This basically means that each service behaves like a
singleton, and their life-cycle is managed by reactor.

Among other things reactor supports validating of dependencies, resetting all services, and overrides of service
factories. These help to write easily testable code, run all tests in a clean state and prevent crashes due to missing
dependencies only detected on their first usage.

Example usage
-------------

### Registering a service
First you need to define the interface of the service:
```cpp
class i_example
{
 public:
   virtual ~i_example() {}

   virtual int add(int a, int b) = 0;
};
```

Then define the implementation:
```cpp
class example_impl : public i_example
{
 public:
   virtual void add(int a, int b) override
   {
      return a+b;
   }
}
```

And finally register the a factory that instantiates the implementation when a service is requested for your interface:
```cpp
static const reactor::factory_registrator<i_example, example_impl> registrator(reactor::prio_normal);
```

### Using a service

First you have to declare a contract to the interface you want to use, preferably it's a global variable in the
compilation unit where the interface will be used:
```cpp
static const reactor::contract<i_example> example_contract;
```

Then wherever you need to use i_example you can acquire a reference to backing service using:
```cpp
auto res = r.get(example_contract).add(1, 1);
```

### Override a service

Let's assume you want to test code that uses i_example and want to replace it's implementation with a mock:
```cpp
const reactor::factory_wrapper_registrator<i_example, true> mock_registrator
   (reactor::prio_test, [&](const std::string &) {
      return example_mock;
   });
```

License
-------
[Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0).