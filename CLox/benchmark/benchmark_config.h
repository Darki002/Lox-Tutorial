#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include "hashtable/insertHighCollision.h"

#define benchmark runInsertHighCollision
#define setUpBenchmark setUpInsertHighCollision
#define freeBenchmark freeInsertHighCollision

#define warmups 3
#define iterations 100

const char* basePath = "C:/Development/Private/Lox-Tutorial/CLox/benchmark";
const char* resultFile = "hashtable/insertHighCollision.txt";
const char* benchmarkName = "Orig 1000";

#endif //BENCHMARK_CONFIG_H
