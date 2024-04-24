cmake_minimum_required(VERSION 3.5)

macro(forward_to_parent vars)
   foreach(_i ${vars})
      set(${_i} ${${_i}} PARENT_SCOPE)
   endforeach()
endmacro()
