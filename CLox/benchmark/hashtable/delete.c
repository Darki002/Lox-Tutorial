#include "delete.h"
#include <stdlib.h>
#include "hashTableBenchmarkUtils.h"

static Table table;
static Value values[COUNT];

void setUpDelete() {
    initTable(&table);
    for (int i = 0; i < COUNT; i++) {
        values[i] = OBJ_VAL(makeRandomStringObj());
        tableSet(&table, values[i], NUMBER_VAL(i));
    }

    shuffleStringObjs(values, COUNT);
}

void freeDelete() {
    for (int i = 0; i < COUNT; i++) {
        free(AS_OBJ(values[i]));
    }
    freeTable(&table);
}

void runDelete() {
    for (int i = 0; i < COUNT; i++) {
        const bool ok = tableDelete(&table, values[i]);

        // optionally branchless consume result to avoid optimizing away
        if (!ok) abort();
    }
}
