#include <stdlib.h>

#include "insertHighCollision.h"

#include "hashTableBenchmarkUtils.h"
#include "../../table.h"

static Table table;
static Value values[COUNT];

void setUpInsertHighCollision() {
    initTable(&table);
    for (int i = 0; i < COUNT; i++) {
        values[i] = OBJ_VAL(makeCollisionStringObj());
    }
}

void freeInsertHighCollision() {
    for (int i = 0; i < COUNT; i++) {
        free(AS_OBJ(values[i]));
    }
    freeTable(&table);
}

void runInsertHighCollision() {
    for (int i = 0; i < COUNT; i++) {
        tableSet(&table, values[i], NUMBER_VAL(i));
    }
}