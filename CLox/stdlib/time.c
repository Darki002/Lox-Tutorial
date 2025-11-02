#include <time.h>
#include "time.h"
#include "../object.h"

#ifdef WIN32
    #include "windows.h"
#endif

bool clockNative(int _, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

static void sleep_ms(int milliseconds)
{
    #ifdef WIN32
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        usleep(milliseconds * 1000);
    #endif
}

bool sleepNative(const int argCount, Value* args) {
    if (argCount != 1) {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'sleep'.", 43));
        return false;
    }

    const Value value = args[0];
    if (!IS_NUMBER(value)) {
        args[-1] = OBJ_VAL(copyString("Expected number as argument for 'sleep'.", 40));
        return false;
    }

    sleep_ms(AS_NUMBER(value) * 1000);
    args[-1] = NIL_VAL;
    return true;
}