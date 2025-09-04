#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include "hashtable/delete.h"

#define benchmark runDelete
#define setUpBenchmark setUpDelete
#define freeBenchmark freeDelete

#define warmups 3
#define iterations 100

const char* basePath = "C:/Development/Private/Lox-Tutorial/CLox/benchmark";
const char* resultFile = "hashtable/delete.txt";
const char* benchmarkName = "Orig";

#endif //BENCHMARK_CONFIG_H
