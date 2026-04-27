# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\DebenGui_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DebenGui_autogen.dir\\ParseCache.txt"
  "DebenGui_autogen"
  )
endif()
