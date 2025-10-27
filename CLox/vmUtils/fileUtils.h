#ifndef CLOX_FILEUTILS_H
#define CLOX_FILEUTILS_H

#include <stdio.h>

#include "../common.h"
#include "../value.h"

ObjString* readLine(FILE* in);

#endif //CLOX_FILEUTILS_H