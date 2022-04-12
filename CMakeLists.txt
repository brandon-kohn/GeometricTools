cmake_minimum_required(VERSION 3.0.2)

# Build Properties
set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "Default relative include path.")

# Targets
project(GTE)

add_library(GTE INTERFACE)
target_include_directories(GTE INTERFACE
  $<BUILD_INTERFACE:${GTE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

# Deployment
install ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/GTE/Mathematics DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GTE)
