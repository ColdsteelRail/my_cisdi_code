# Install script for directory: /home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/home/dcn-space/myworkspace/my_cisdi_code/build/release-install-cpp11")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/dcn-space/myworkspace/my_cisdi_code/build/release-cpp11/lib/libmuduo_base.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Logging.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/StringPiece.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/BlockingQueue.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/ThreadLocalSingleton.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/FileUtil.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Date.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Singleton.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/copyable.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/ProcessInfo.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Types.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Atomic.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/LogFile.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/LogStream.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/CurrentThread.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Condition.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Timestamp.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Mutex.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Exception.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/Thread.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/TimeZone.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/ThreadPool.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/BoundedBlockingQueue.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/ThreadLocal.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/noncopyable.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/CountDownLatch.h"
    "/home/dcn-space/myworkspace/my_cisdi_code/linux_multithread_server/muduo/base/AsyncLogging.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/dcn-space/myworkspace/my_cisdi_code/build/release-cpp11/muduo/base/test/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

