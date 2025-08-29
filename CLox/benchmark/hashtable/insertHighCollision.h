#ifndef CLOX_INSERT_H
#define CLOX_INSERT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../object.h"
#include "../../table.h"

#define COUNT 1000

extern Table table;
extern Value values[COUNT];

void setUp();
void freeBench();
void run();

#endif //CLOX_INSERT_H