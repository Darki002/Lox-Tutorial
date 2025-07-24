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
)
