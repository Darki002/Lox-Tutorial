#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "benchmark_config.h"

#include "hashtable/insert_string_literals.h"

typedef void (*bench_func)();

void run_benchmark() {
    LARGE_INTEGER freq_li;
    QueryPerformanceFrequency(&freq_li);
    const double freq = (double)freq_li.QuadPart; // ticks per second

    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, resultFile);

    HANDLE thread = GetCurrentThread();
    const DWORD_PTR oldMask = SetThreadAffinityMask(thread, 1);

    // --- Warmup ---
    for (int i = 0; i < warmups; i++) {
        for (int k = 0; k < innerLoops; k++) runBenchmark();
    }

    double total_ns = 0.0;
    double min_ns = 1e300, max_ns = 0.0;

    for (int i = 0; i < iterations; i++) {
        LARGE_INTEGER s, e;
        QueryPerformanceCounter(&s);

        for (int k = 0; k < innerLoops; k++) runBenchmark();

        QueryPerformanceCounter(&e);

        double elapsed_s = (double)(e.QuadPart - s.QuadPart) / freq;
        double ns = elapsed_s * 1e9;

        if (ns < min_ns) min_ns = ns;
        if (ns > max_ns) max_ns = ns;
        total_ns += ns;
    }

    if (oldMask) SetThreadAffinityMask(thread, oldMask);

    const double avg_ns = total_ns / iterations;
    const double per_op_avg_ns = avg_ns / innerLoops;
    const double ops_per_sec = 1e9 / per_op_avg_ns;

    const time_t now = time(nullptr);
    struct tm local;
    localtime_s(&local, &now);

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local);

    printf("Warmup: %d, Iterations: %d, Inner loops: %d\n", warmups, iterations, innerLoops);
    printf("Batch time (ns): min=%.0f  max=%.0f  avg=%.0f\n", min_ns, max_ns, avg_ns);
    printf("Per-op avg (ns): %.2f\n", per_op_avg_ns);
    printf("Throughput (ops/s): %.0f\n", ops_per_sec);

    FILE* f = fopen(fullPath, "a");
    if (f) {
        fprintf(f, "Benchmark Results (%s) \n", benchmarkName);
        fprintf(f, "Timestamp: [%s]\n", timestamp);
        fprintf(f, "Warmup: %d, Iterations: %d, Inner loops: %d\n", warmups, iterations, innerLoops);
        fprintf(f, "Batch time (ns): min=%.0f  max=%.0f  avg=%.0f\n", min_ns, max_ns, avg_ns);
        fprintf(f, "Per-op avg (ns): %.2f\n", per_op_avg_ns);
        fprintf(f, "Throughput (ops/s): %.0f\n", ops_per_sec);
        fprintf(f, "--------------------------------------\n");
        fclose(f);
    } else {
        fprintf(stderr, "Error: could not open file %s for writing.\n", resultFile);
    }
}

int main() {
    setUpBenchmark();
    run_benchmark();
    freeBenchmark();
    return 0;
}
