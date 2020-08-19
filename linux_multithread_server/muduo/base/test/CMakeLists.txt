add_executable(timestamp_unittest Timestamp_unittest.cc)
target_link_libraries(timestamp_unittest muduo_base)

add_executable(atomic_unittest Atomic_unittest.cc)
# 不需要，Atomic只含头文件，已包含target_link_libraries(atomic_unittest muduo_base)

#add_test(NAME <name> [CONFIGURATIONS [Debug|Release|...]]
#           [WORKING_DIRECTORY dir]
#           COMMAND <command> [arg1 [arg2 ...]])

# 其中通过NAME关键值指定本测试的名称，可以随意命名，
# Debug/Release选项可以控制在不同的编译版本下是否进行测试。
# 通过WORKING_DIRECTORY可以设置工作路径，command表示可运行程序
add_test(NAME atomic_unittest COMMAND atomic_unittest)

add_executable(exception_test Exception_test.cc)
target_link_libraries(exception_test muduo_base)

add_executable(thread_test Thread_test.cc)
target_link_libraries(thread_test muduo_base)

add_executable(mutex_test Mutex_test.cc)
target_link_libraries(mutex_test muduo_base)

add_executable(countDownLatch_test CountDownLatch_test.cc)
target_link_libraries(countDownLatch_test muduo_base)

add_executable(blockingQueue_test BlockingQueue_test.cc)
target_link_libraries(blockingQueue_test muduo_base)

add_executable(blockingQueue_bench BlockingQueue_bench.cc)
target_link_libraries(blockingQueue_bench muduo_base)

add_executable(boundedBlockingQueue_test BoundedBlockingQueue_test.cc)
target_link_libraries(boundedBlockingQueue_test muduo_base)

add_executable(threadPool_test ThreadPool_test.cc)
target_link_libraries(threadPool_test muduo_base)

add_executable(singleton_test Singleton_test.cc)
target_link_libraries(singleton_test muduo_base)

add_executable(threadLocal_test ThreadLocal_test.cc)
target_link_libraries(threadLocal_test muduo_base)

add_executable(threadLocalSingleton_test ThreadLocalSingleton_test.cc)
target_link_libraries(threadLocalSingleton_test muduo_base)

add_executable(asyncLogging_test AsyncLogging_test.cc)
target_link_libraries(asyncLogging_test muduo_base)

if(BOOSTTEST_LIBRARY) # boost单元测试框架，test.cc文件中没有main函数，只有用例
add_executable(logStream_test LogStream_test.cc)
target_link_libraries(logStream_test muduo_base boost_unit_test_framework)
add_test(NAME logStream_test COMMAND logStream_test)
endif()

add_subdirectory(testlog)