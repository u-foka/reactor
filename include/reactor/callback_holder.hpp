#ifndef CALLBACK_HOLDER_HPP
#define CALLBACK_HOLDER_HPP

#include <functional>
#include <map>
#include <shared_mutex>

namespace iws {

template<typename... Args>
class callback_holder
{
 public:
   typedef std::function<void(Args...)> value_type;

   callback_holder()
         : _next_id(0)
         , _list()
         , _mutex()
   {
   }

   size_t connect(value_type cb)
   {
      std::unique_lock<std::shared_mutex> lock(_mutex);

      _list[_next_id] = cb;
      return _next_id++;
   }

   bool disconnect(size_t id)
   {
      std::unique_lock<std::shared_mutex> lock(_mutex);

      auto it = _list.find(id);
      if (it != _list.end())
      {
         _list.erase(it);
         return true;
      }

      return false;
   }

   void clear()
   {
      std::unique_lock<std::shared_mutex> lock(_mutex);
      _list.clear();
   }

   void operator()(Args &&... args)
   {
      std::shared_lock<std::shared_mutex> lock(_mutex);

      for (auto it : _list)
      {
         (it.second)(std::forward<Args>(args)...);
      }
   }

   size_t callback_count()
   {
      std::shared_lock<std::shared_mutex> lock(_mutex);
      return _list.size();
   }

   operator bool()
   {
      std::shared_lock<std::shared_mutex> lock(_mutex);
      return 0 != _list.size();
   }

 private:
   size_t _next_id;
   std::map<size_t, value_type> _list;
   std::shared_mutex _mutex;
};

} // namespace iws

#endif // CALLBACK_HOLDER_HPP
