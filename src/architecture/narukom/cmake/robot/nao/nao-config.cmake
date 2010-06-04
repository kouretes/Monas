set (CMAKE_MODULE_PATH ${TOOLCHAIN_DIR}/cmake)
include(${ConfigDir}/${TargetPlatform}/bootstrap.cmake)

set(CTC_DIR ${TOOLCHAIN_DIR})
