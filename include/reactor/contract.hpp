#ifndef __IWS_REACTOR_CONTRACT_HPP__
#define __IWS_REACTOR_CONTRACT_HPP__

#include "typed_contract.hpp"

#include "index.hpp"

namespace iws::reactor {

template<typename T>
class contract : public typed_contract<T>
{
 public:
   contract(const std::string &instance = std::string());
   contract(reactor *r_inst, const std::string &instance = std::string());
   virtual ~contract();
   virtual const index &get_index() const override;

 private:
   const index _index;
};

// ----

template<typename T>
contract<T>::contract(const std::string &instance)
      : typed_contract<T>()
      , _index(typeid(T), instance)
{}

template<typename T>
contract<T>::contract(reactor *r_inst, const std::string &instance)
      : typed_contract<T>(r_inst)
      , _index(typeid(T), instance)
{}

template<typename T>
contract<T>::~contract()
{}

template<typename T>
const index &contract<T>::get_index() const
{
   return _index;
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_CONTRACT_HPP__
