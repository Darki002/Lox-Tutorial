#include <time.h>
#include "time.h"

bool clockNative(int _, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}