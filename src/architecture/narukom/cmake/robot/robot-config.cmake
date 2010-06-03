set (CMAKE_MODULE_PATH ${TOOLCHAIN_DIR}/cmake)
include(${ConfigDir}/${TargetPlatform}/bootstrap.cmake)
#use_lib(BOOST_DATE_TIME, BOOST_THREAD,BOOST_SYSTEM)
set(CTC_DIR ${TOOLCHAIN_DIR})
