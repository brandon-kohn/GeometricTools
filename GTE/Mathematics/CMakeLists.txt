project(gtmathematics VERSION ${GTE_VERSION_MAJOR}.${GTE_VERSION_MINOR})

add_library(gtmathematics INTERFACE)
target_include_directories(gtmathematics INTERFACE
  $<BUILD_INTERFACE:${gtmathematics_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

# Deployment
install ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GeometricTools/GTE/Mathematics)
