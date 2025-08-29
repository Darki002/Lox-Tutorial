#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include "hashtable/insertHighCollision.h"

#define benchmark run
#define setUpBenchmark setUp
#define freeBenchmark freeBench

#define warmups 3
#define iterations 30
#define innerLoops 100

const char* basePath = "C:/Development/Private/Lox-Tutorial/CLox/benchmark";
const char* resultFile = "hashtable/high-collission.txt";
const char* benchmarkName = "Orig 10000";

#endif //BENCHMARK_CONFIG_H
