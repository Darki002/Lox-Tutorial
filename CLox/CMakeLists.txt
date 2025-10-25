cmake_minimum_required(VERSION 3.31)
project(CLox C)

set(CMAKE_C_STANDARD 99)

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
)