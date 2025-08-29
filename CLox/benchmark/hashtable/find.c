#include "find.h"

#include <stdlib.h>

#include "hashTableBenchmarkUtils.h"

static Table table;
static Value values[COUNT];

void setUpFind() {
    initTable(&table);
    for (int i = 0; i < COUNT; i++) {
        values[i] = OBJ_VAL(makeRandomStringObj());
        tableSet(&table, values[i], NUMBER_VAL(i));
    }

    shuffleStringObjs(values, COUNT);
}

void freeFind() {
    for (int i = 0; i < COUNT; i++) {
        free(AS_OBJ(values[i]));
    }
    freeTable(&table);
}

void runFind() {
    for (int i = 0; i < COUNT; i++) {
        Value out;
        const bool ok = tableGet(&table, values[i], &out);

        // optionally branchless consume result to avoid optimizing away
        if (!ok) abort();
    }
}
