#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include "hashtable/find.h"

#define benchmark runFind
#define setUpBenchmark setUpFind
#define freeBenchmark freeFind

#define warmups 3
#define iterations 100

const char* basePath = "C:/Development/Private/Lox-Tutorial/CLox/benchmark";
const char* resultFile = "hashtable/find.txt";
const char* benchmarkName = "Orig";

#endif //BENCHMARK_CONFIG_H
