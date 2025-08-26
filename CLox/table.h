#ifndef clox_table_h
#define clox_table_h

#include <math.h>

#include "common.h"
#include "value.h"

typedef struct {
    Value key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(const Table* table, Value key, Value* value);
bool tableSet(Table* table, Value key, Value value);
bool tableDelete(const Table* table, Value key);
void tableAddAll(const Table* from, Table* to);
ObjString* tableFindString(const Table* table, const char* chars, int length, uint32_t hash);

static const int primes[] = {
    3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
          1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
          17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
          187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
          1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
};

static constexpr size_t primesCount = sizeof(primes) / sizeof(primes[0]);

static inline bool isPrime(const int candidate) {
    if ((candidate & 1) == 0) return candidate == 2;

    const int limit = sqrt(candidate);
    for (int i = 3; i < limit; i++) {
        if (candidate % i == 0) {
            return false;
        }
    }
    return true;
}

static inline int getPrime(const int min) {
    for (int i = 0; i < primesCount; i++) {
        if (primes[i] >= min) {
            return primes[i];
        }
    }

    for (int i = min | 1; i < INT32_MAX; i += 2) { // (min | 1) to make sure it is odd
        if (isPrime(i)) return i;
    }

    return INT32_MAX;
}

#define GROW_PRIME(oldPrime) getPrime(oldPrime * 2)

#endif //clox_table_h
