#ifndef __IWS_REACTOR_FACTORY_WRAPPER_HPP__
#define __IWS_REACTOR_FACTORY_WRAPPER_HPP__

#include "factory_base.hpp"

#include <memory>

namespace iws::reactor {

/**
 * @brief factory wrapper for custom factory logic
 *
 * This factory is constructed with a functor that is called every time a new object has to created.
 * Thorugh the functor, any custom logic can be added to the object creation.
 *
 * Template parameters:
 * - I: The returned type (preferably an interface class).
 */
template<typename I>
class factory_wrapper : public factory_base
{
 public:
   typedef std::function<std::shared_ptr<I>(const std::string &)> producer_function;
   /**
    * @brief factory_wrapper constructor
    * @param producer is the functor that creates a new I* object
    */
   factory_wrapper(const producer_function &producer);
   /**
    * @brief produces a new object
    * @param instance is the name of the instance to be created
    * @return returns a new object of type I wrapped into an shared_ptr
    */
   virtual factory_result produce(const std::string &instance) const override;

 private:
   producer_function _producer;
};

// ----

template<typename I>
factory_wrapper<I>::factory_wrapper(const producer_function &producer)
      : factory_base(typeid(I))
      , _producer(producer)
{
}

template<typename I>
factory_result factory_wrapper<I>::produce(const std::string &instance) const
{
   return _producer(instance);
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_FACTORY_WRAPPER_HPP__
