#ifndef REACTOR_UTILS_HPP
#define REACTOR_UTILS_HPP

#include <algorithm>

/* Unused variable helper */
#define UNUSED(X) (void)(X)

/* Stringify helper */
#define X_MACRO_STR(s) #s
#define MACRO_STR(s) X_MACRO_STR(s)

namespace iws {
namespace reactor_utils {

template<typename Container>
typename Container::iterator find(Container &container, const typename Container::value_type &what)
{
   return std::find(container.begin(), container.end(), what);
}

template<typename Container>
typename Container::const_iterator find(const Container &container, const typename Container::value_type &what)
{
   return std::find(container.cbegin(), container.cend(), what);
}

template<typename Container, typename UnaryPredicate>
typename Container::iterator find_if(Container &container, const UnaryPredicate &pred)
{
   return std::find_if(container.begin(), container.end(), pred);
}

template<typename Container, typename UnaryPredicate>
typename Container::const_iterator find_if(const Container &container, const UnaryPredicate &pred)
{
   return std::find_if(container.cbegin(), container.cend(), pred);
}

template<typename ContainerT, typename PredicateT>
void erase_if(ContainerT &items, const PredicateT &predicate)
{
   for (auto it = items.begin(); it != items.end();)
   {
      if (predicate(*it))
         it = items.erase(it);
      else
         ++it;
   }
}

} // namespace reactor_utils
} // namespace iws

#endif // REACTOR_UTILS_HPP