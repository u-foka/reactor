#ifndef __IWS_REACTOR_ADDON_REGISTRATOR_HPP__
#define __IWS_REACTOR_ADDON_REGISTRATOR_HPP__

namespace iws::reactor {

template<typename T, bool unregister = false>
class addon_registrator
{
 public:
   addon_registrator(priorities priority, addon<T> &&inst);
   addon_registrator(const std::string &instance, priorities priority, addon<T> &&inst);

   ~addon_registrator();

 private:
   const std::string _name;
   const priorities _priority;
};

// ----

template<typename T, bool unregister>
addon_registrator<T, unregister>::addon_registrator(priorities priority, addon<T> &&inst)
      : _name(std::string())
      , _priority(priority)
{
   r.register_addon(_name, _priority, std::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
addon_registrator<T, unregister>::addon_registrator(
      const std::string &instance, priorities priority, addon<T> &&inst)
      : _name(instance)
      , _priority(priority)
{
   r.register_addon(_name, _priority, std::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
addon_registrator<T, unregister>::~addon_registrator()
{
   if (unregister)
   {
      r.unregister_addon(_name, _priority, typeid(T));
   }
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_ADDON_REGISTRATOR_HPP__
