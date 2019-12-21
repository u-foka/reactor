#ifndef __IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__
#define __IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__

#include <exception>
#include <sstream>
#include <string>

namespace iws::reactor {

class not_registred_exception : public std::exception
{
 public:
   not_registred_exception(const std::string &category, const std::type_info &type, const std::string &name);
   virtual const char *what() const noexcept;

 private:
   const std::string &_category;
   const std::type_info &_type;
   const std::string _name;
   mutable std::string _msg_buffer;
};

class factory_not_registred_exception : public not_registred_exception
{
 public:
   factory_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

class addon_not_registred_exception : public not_registred_exception
{
 public:
   addon_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

class addon_filter_not_registred_exception : public not_registred_exception
{
 public:
   addon_filter_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

} // namespace iws::reactor

#endif //__IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__
