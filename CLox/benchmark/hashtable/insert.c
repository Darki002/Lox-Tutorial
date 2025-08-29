#include "insert.h"

#include <stdlib.h>

#include "hashTableBenchmarkUtils.h"

static Table table;
static Value values[COUNT];

void setUpInsert() {
    initTable(&table);
    for (int i = 0; i < COUNT; i++) {
        values[i] = OBJ_VAL(makeRandomStringObj());
    }
}

void freeInsert() {
    for (int i = 0; i < COUNT; i++) {
        free(AS_OBJ(values[i]));
    }
}

void runInsert() {
    for (int i = 0; i < COUNT; i++) {
        tableSet(&table, values[i], NUMBER_VAL(i));
    }
}