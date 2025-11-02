#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "benchmark_config.h"

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

typedef void (*bench_func)();

int main() {
    LARGE_INTEGER freq_li;
    QueryPerformanceFrequency(&freq_li);
    const double freq = (double)freq_li.QuadPart; // ticks per second

    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, resultFile);

    HANDLE thread = GetCurrentThread();
    const DWORD_PTR oldMask = SetThreadAffinityMask(thread, 1);

    // --- Warmup ---
    for (int i = 0; i < warmups; i++) {
        setUpBenchmark();
        benchmark();
        freeBenchmark();
    }

    double total_ns = 0.0;
    double min_ns = 1e300, max_ns = 0.0;

    for (int i = 0; i < iterations; i++) {
        setUpBenchmark();

        LARGE_INTEGER s, e;
        QueryPerformanceCounter(&s);

        benchmark();

        QueryPerformanceCounter(&e);

        freeBenchmark();

        double elapsed_s = (double)(e.QuadPart - s.QuadPart) / freq;
        double ns = elapsed_s * 1e9;

        if (ns < min_ns) min_ns = ns;
        if (ns > max_ns) max_ns = ns;
        total_ns += ns;
    }

    if (oldMask) SetThreadAffinityMask(thread, oldMask);

    const double avg_ns = total_ns / iterations;
    const double ops_per_sec = 1e9 / avg_ns;

    const time_t now = time(NULL);
    struct tm local;
    localtime_s(&local, &now);

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local);

    printf("Warmup: %d, Iterations: %d\n", warmups, iterations);
    printf("Batch time (ns): min=%.0f  max=%.0f  avg=%.0f\n", min_ns, max_ns, avg_ns);
    printf("Per-op avg (ns): %.2f\n", avg_ns);
    printf("Throughput (ops/s): %.0f\n", ops_per_sec);

    FILE* f = fopen(fullPath, "a");
    if (f) {
        fprintf(f, "Benchmark Results (%s) \n", benchmarkName);
        fprintf(f, "Timestamp: [%s]\n", timestamp);
        fprintf(f, "Warmup: %d, Iterations: %d\n", warmups, iterations);
        fprintf(f, "Batch time (ns): min=%.0f  max=%.0f  avg=%.0f\n", min_ns, max_ns, avg_ns);
        fprintf(f, "Per-op avg (ns): %.2f\n", avg_ns);
        fprintf(f, "Throughput (ops/s): %.0f\n", ops_per_sec);
        fprintf(f, "--------------------------------------\n");
        fclose(f);
    } else {
        fprintf(stderr, "Error: could not open file %s for writing.\n", resultFile);
    }

    return 0;
}
