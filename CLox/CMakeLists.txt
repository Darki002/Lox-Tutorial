cmake_minimum_required(VERSION 3.31)
project(CLox C)

set(CMAKE_C_STANDARD 99)

if (MSVC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
endif()

add_executable(CLox clox.c
        common.h
        chunk.h
        chunk.c
        memory.h
        memory.c
        debug.h
        debug.c
        value.h
        value.c
        vm.c
        vm.h
        compiler.c
        compiler.h
        scanner.c
        scanner.h
        object.h
        object.c
        table.c
        table.h
        global.c
        global.h
        utils/io.c
        utils/io.h
        utils/coerce.c
        utils/coerce.h
        stdlib/time.c
        stdlib/time.h
        stdlib/nativeIo.c
        stdlib/nativeIo.h
        stdlib/nativeErr.c
        stdlib/nativeErr.h
        stdlib/cast.c
        stdlib/cast.h
)
add_executable(Benchmark benchmark/benchmark.c
        common.h
        chunk.h
        chunk.c
        memory.h
        memory.c
        debug.h
        debug.c
        value.h
        value.c
        vm.c
        vm.h
        compiler.c
        compiler.h
        scanner.c
        scanner.h
        object.h
        object.c
        table.c
        table.h
        benchmark/hashtable/insertHighCollision.c
        benchmark/hashtable/insertHighCollision.h
        benchmark/benchmark_config.h
        benchmark/hashtable/insert.c
        benchmark/hashtable/insert.h
        benchmark/hashtable/hashTableBenchmarkUtils.c
        benchmark/hashtable/hashTableBenchmarkUtils.h
        benchmark/hashtable/find.c
        benchmark/hashtable/find.h
        benchmark/hashtable/delete.c
        benchmark/hashtable/delete.h
        stdlib/cast.c
        stdlib/cast.h
)