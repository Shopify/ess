cmake_minimum_required(VERSION 3.6)
project(enterprise_script_service)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
        ext/enterprise_script_service/msgpack/include
        ext/enterprise_script_service/mruby/include
        ext/enterprise_script_service/libseccomp/include
        ext/enterprise_script_service
)

add_definitions(
        -DMRB_ENABLE_DEBUG_HOOK
        -DMRB_INT64
        -DMRB_UTF8_STRING
        -DMRB_WORD_BOXING
        -DYYDEBUG
)

set(SOURCE_FILES
    ext/enterprise_script_service/data.cpp
    ext/enterprise_script_service/data.hpp
    ext/enterprise_script_service/dlmalloc.cpp
    ext/enterprise_script_service/dlmalloc.hpp
    ext/enterprise_script_service/dlmalloc_config.hpp
    ext/enterprise_script_service/error.cpp
    ext/enterprise_script_service/error.hpp
    ext/enterprise_script_service/memory_pool.cpp
    ext/enterprise_script_service/memory_pool.hpp
    ext/enterprise_script_service/mruby_engine.cpp
    ext/enterprise_script_service/mruby_engine.hpp
    ext/enterprise_script_service/sandbox.cpp
    ext/enterprise_script_service/sandbox.hpp
    ext/enterprise_script_service/timer.cpp
    ext/enterprise_script_service/timer.hpp
    ext/enterprise_script_service/units.hpp
    ext/enterprise_script_service/script_data.cpp
    ext/enterprise_script_service/script_data.hpp
    ext/enterprise_script_service/script_runner.cpp
    ext/enterprise_script_service/script_runner.hpp
    ext/enterprise_script_service/options.hpp
    ext/enterprise_script_service/options.cpp
)

set(TEST_FILES
    tests/timer_test.cpp
    tests/data_writer_test.cpp
    tests/script_data_test.cpp
    tests/mruby_data_writer_test.cpp
    tests/integration_test.cpp
    tests/script_runner_test.cpp
    tests/options_test.cpp
)

set(MPDECIMAL
    ext/enterprise_script_service/mruby-mpdecimal/src/basearith.c
    ext/enterprise_script_service/mruby-mpdecimal/src/basearith.h
    ext/enterprise_script_service/mruby-mpdecimal/src/bits.h
    ext/enterprise_script_service/mruby-mpdecimal/src/constants.c
    ext/enterprise_script_service/mruby-mpdecimal/src/constants.h
    ext/enterprise_script_service/mruby-mpdecimal/src/context.c
    ext/enterprise_script_service/mruby-mpdecimal/src/convolute.c
    ext/enterprise_script_service/mruby-mpdecimal/src/convolute.h
    ext/enterprise_script_service/mruby-mpdecimal/src/crt.c
    ext/enterprise_script_service/mruby-mpdecimal/src/crt.h
    ext/enterprise_script_service/mruby-mpdecimal/src/difradix2.c
    ext/enterprise_script_service/mruby-mpdecimal/src/difradix2.h
    ext/enterprise_script_service/mruby-mpdecimal/src/ext.c
    ext/enterprise_script_service/mruby-mpdecimal/src/fnt.c
    ext/enterprise_script_service/mruby-mpdecimal/src/fnt.h
    ext/enterprise_script_service/mruby-mpdecimal/src/fourstep.c
    ext/enterprise_script_service/mruby-mpdecimal/src/fourstep.h
    ext/enterprise_script_service/mruby-mpdecimal/src/io.c
    ext/enterprise_script_service/mruby-mpdecimal/src/io.h
    ext/enterprise_script_service/mruby-mpdecimal/src/memory.c
    ext/enterprise_script_service/mruby-mpdecimal/src/memory.h
    ext/enterprise_script_service/mruby-mpdecimal/src/mpdecimal.c
    ext/enterprise_script_service/mruby-mpdecimal/src/mpdecimal.h
    ext/enterprise_script_service/mruby-mpdecimal/src/mpsignal.c
    ext/enterprise_script_service/mruby-mpdecimal/src/numbertheory.c
    ext/enterprise_script_service/mruby-mpdecimal/src/numbertheory.h
    ext/enterprise_script_service/mruby-mpdecimal/src/sixstep.c
    ext/enterprise_script_service/mruby-mpdecimal/src/sixstep.h
    ext/enterprise_script_service/mruby-mpdecimal/src/transpose.c
    ext/enterprise_script_service/mruby-mpdecimal/src/transpose.h
    ext/enterprise_script_service/mruby-mpdecimal/src/typearith.h
    ext/enterprise_script_service/mruby-mpdecimal/src/umodarith.h
)

add_library(mruby-mpdecimal STATIC ${MPDECIMAL})

add_executable(enterprise_script_service
        ${SOURCE_FILES}
        ext/enterprise_script_service/ext.cpp
)

add_subdirectory(tests/googletest)
add_executable(enterprise_script_service_tests
        ${SOURCE_FILES}
        ${TEST_FILES}
)

target_link_libraries(
        enterprise_script_service ${CMAKE_SOURCE_DIR}/ext/enterprise_script_service/mruby/build/sandbox/lib/libmruby.a
)

target_link_libraries(
        enterprise_script_service_tests
        ${CMAKE_SOURCE_DIR}/ext/enterprise_script_service/mruby/build/host/lib/libmruby.a
        gtest
        gtest_main
)
