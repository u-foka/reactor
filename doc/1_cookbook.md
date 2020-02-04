\page cookbook CookBook

# Concepts

Reactor bridges the gap between your decoupled objects. You register an implementation to an interface, and the user of that interface can simply query the implementation without knowing it directly.

## Structuring your code

With reactor you can have your implementation in a single file without a separate header, as no direct inclusions of it is necessary unless you want to inherit from it.

Only the interface has to be provided as a header, and your user can get an instance that implements that interface without directly referencing your implementation. This way you can get rid of the overhead maintaining two headers when changing your public interface.

Reactor provides multiple convinience headers so you can include the necessary parts when you write a provider or a user.
- \link include/reactor/client.hpp reactor/client.hpp \endlink
- \link include/reactor/provider.hpp reactor/provider.hpp \endlink
- \link include/reactor/provider_ex.hpp reactor/provider_ex.hpp \endlink

The examples are also structured this way.

## Runtime behavior

Reactor uses a technique - called nifty counter or Schwarz counter - to make sure it's global
\link iws::reactor::r
   r
\endlink
instance is available in every compilation unit - that includes reactor/r.h even in static initialization and destruction phases.

(_What it can't control is the dll load order in windows environment while linked dynamically. If you want to share a global r object among multiple dlls you have to be really careful about that order_)

This behavior is important to be able to use the provided registrators and contracts as static globals.

When using reactor the static init phase is generally reserved for registrations, you should not consume anything from reactor as it might not have been registered yet.

The registered services are created on their first access, afterwards the same instance is always returned (unless
\link iws::reactor::reactor::reset_objects()
   reset_objects()
\endlink
 is called) until the termination of the application.

 It is guaranteed the reactor will release it's references to all held objects in the reverse order of their creation. If you want to use a dependency within your destructor it's a good practice to store a shared_ptr reference to that object using
\link iws::reactor::reactor::get_ptr()
   get_ptr()
\endlink
to extend it's lifetime even after when reactor has already released it.

You can also connect to the
\link iws::reactor::reactor::sig_before_reset_objects
   sig_before_reset_objects
\endlink
signal of reactor to do the necessary cleanup steps with your dependencies before they are destroyed.

# Contracts

A contract expresses a dependency to an inmplementation. It is recommended to create all your contracts as static global objects in each compilation unit.

This way an application can tell that all contracts are fulfilled with a registered factory any time after the static init phase is finished.

To avoid random crashes during runtime caused by missing dependencies, you should
\link iws::reactor::reactor::validate_contracts()
   validate_contracts()
\endlink
at the beginning of main() in your programs, or even check it from your CI.

# Addons

With addons you can basically register a callback to a not (yet) existing object. This way you can avoid circular dependencies between your objects, and decouple the lifetime of the object from the callback registration when necessary.

# Registration

Services are registered via factories. A factory can be registered via 
\link iws::reactor::reactor::register_factory()
   reactor::register_factory()
\endlink
however you should use the builtin factory templates and registrators.

## Simple

The simplest way to register your service is via
\link iws::reactor::factory_registrator
   factory_registrator
\endlink
as follows.

```cpp
#include <reactor/provider.hpp>

#include "i_example.hpp"

// ...

namespace reactor = iws::reactor;

// ...

class example_impl : public i_example
{

// ...

};

static const reactor::factory_registrator<i_example, example_impl> registrator(reactor::prio_normal);
```

This will register an instance of
\link iws::reactor::factory
   factory
\endlink
parametrized for the provided types.

## Constructor arguments

The builtin factory can also store and use constructor arguments for object creation.

If your objects constructor needs arguments, you can register a factory that passes the necessary arguments.

```cpp
class example_impl : public i_example
{
 public:
   example(int arg1, const std::string &arg2);

// ...

};

static const reactor::factory_registrator<i_example, example_impl> registrator(reactor::prio_normal, 42, std::string("something"));
```

# Usage

Now as you have an interface and a registered factory for it, the next step is to use your service.

## Simple

First you should create a
\link iws::reactor::contract
   contract
\endlink
in your compilation unit, then you can use
\link iws::reactor::reactor::get()
   get()
\endlink
to acquire a reference to the reactor managed implementation of the given interface.

```cpp
#include <reactor/client.hpp>

#include "i_example.hpp"

// ...

namespace reactor = iws::reactor;
using reactor::r;

// ...

static const reactor::contract<i_example> example_contract;

//...

void sample_user()
{
   r.get(example_contract).example_function();
}
```

## Pulley (the even more simple way)

It's generally a good idea to store dependency references within your constructor when you want to use them multiple times during the life of your object.

\link iws::reactor::pulley
   pulley
\endlink
makes this type of use extremely simple

```cpp
#include <reactor/client.hpp>

#include "i_example.hpp"

// ...

namespace reactor = iws::reactor;

// ...

class client {

// ...

 private:
   reactor::pulley<i_example> _example;
};

// ...

void client::fun()
{
   _example->example_function();
}
```

Internally it also creates a global contract for you, so the
\link iws::reactor::reactor::validate_contracts()
   contract validation
\endlink
will also work.

# Named instances

There are cases when you need multiple instances of the same service for different purposes. To support this use-case reactor supports named instances.

To acquire a named instance, you can add the name to your contracts constructor.

```cpp
static const reactor::contract<i_example> instance1_contract("instance1");
```

When there is no instance name is provided, the default instance is referenced (that is identified by an empty string)

You can also provide an instance specific implementation by registering your factory with instance name.

```cpp
static const reactor::factory_registrator<i_example, example_impl> registrator("instance1", reactor::prio_normal);
```

While getting an named instance, reactor first looks for a name specific registration and if it doesn't exists then tries to the default factory.

More about named instances (like how to pass the name into your constructor) in the \ref advanced_named_instance section on the \ref advanced page.

# Custom factories

When you need to have more precise control over the creation of your instance, you can register a factory wrapper and provide your own code to create the new object.

```cpp
const reactor::factory_wrapper_registrator<i_example> registrator(reactor::prio_normal,
   [&](const std::string &name) {
      std::cout << "Creating example with instance name" << name << std::endl;
      auto obj = std::make_shared<example>();

      // do any necessary initialization on obj before returning

      return obj;
   });
```

_Note that it also gets the instance name that is requested from reactor, so you can also incorporate it into your creation logic_

# Priorities

You can register your factories for the same interface with different priorities (one interface-name-priority combination can only registered once) and always the factory with the highest priority will be used to produce a new instance if necessary.

(existing objects are preserved when registering an override factory, so if the object is already created the override might be ineffective)

So in the below example the first implementation will be used.
```cpp
static const reactor::factory_registrator<i_example, example_impl_first> registrator(reactor::prio_override);
static const reactor::factory_registrator<i_example, example_impl_second> registrator(reactor::prio_normal);

static const reactor::contract<i_example> example_contract;
```

# Addons