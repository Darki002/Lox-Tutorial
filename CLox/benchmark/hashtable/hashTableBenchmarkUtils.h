#ifndef HASHTABLEBENCHMARKUTILS_H
#define HASHTABLEBENCHMARKUTILS_H

#include "../../object.h"
#include "../../table.h"

void randomString(char *str, size_t length);
ObjString* makeRandomStringObj();
ObjString* makeCollisionStringObj();

#endif //HASHTABLEBENCHMARKUTILS_H
