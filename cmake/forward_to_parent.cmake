cmake_minimum_required(VERSION 3.1)

macro(forward_to_parent vars)
   foreach(_i ${vars})
      set(${_i} ${${_i}} PARENT_SCOPE)
   endforeach()
endmacro()
