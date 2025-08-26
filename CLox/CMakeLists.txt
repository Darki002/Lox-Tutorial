cmake_minimum_required(VERSION 3.31)
project(CLox C)

set(CMAKE_C_STANDARD 23)

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
        benchmark/hashtable/insert_string_literals.c
        benchmark/hashtable/insert_string_literals.h
)