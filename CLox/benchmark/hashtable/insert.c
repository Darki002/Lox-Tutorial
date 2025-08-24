#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "insert.h"
#include "../../table.h"

Table table;
Value values[];

void setUp() {
    initTable(&table);

    const char* path = "C:/Developement/Lox-Tutorial/CLox/benchmark/hashtable/string_literals.txt";
    const int loaded = loadStringsFromFile(path, values, COUNT);
    printf("Loaded %d strings into values[]\n", loaded);

    for (int i = loaded; i < COUNT; i++) {
        values[i] = values[i % (loaded > 0 ? loaded : 1)];
    }
}

void freeBench() {
    for (int i = 0; i < 1000; i++) {
        free(AS_OBJ(values[i]));
    }
}

void run() {
    for (int i = 0; i < 1000; i++) {
        tableSet(&table, values[i], NIL_VAL);
    }
}