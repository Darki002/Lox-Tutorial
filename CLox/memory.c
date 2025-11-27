#include "memory.h"
#include <stddef.h>
#include <stdlib.h>

#include "object.h"
#include "value.h"
#include "table.h"
#include "vm.h"
#include "table.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#endif // DEBUG_LOG_GC

#define GC_HEAP_GROW_FACTOR 2

/*
 oldSize | newSize | Operation
 0 | Non-zero | Allocate new block
 Non-zero | 0 | Free allocation
 Non-Zero | Smaller than oldSize | Shrink existing allocation
 Non-zero | Larger than oldSize | Grow existing allocation.
 */
void* reallocate(void *pointer, size_t oldSize, const size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void *result = realloc(pointer, newSize);
  if (result == NULL)
    exit(1);
  return result;
}

static void freeObject(Obj *object) {
#ifdef DEBUG_LOG_GC
  printf("%p free type %d\n", (void *)object, objType(object));
#endif // DEBUG_LOG_GC

  switch (objType(object)) {
    case OBJ_CLOSURE: {
        const ObjClosure *closure = (ObjClosure *)object;
        removeReference((Obj*)closure->function);
        
        for (int i = 0; i < closure->upvalueCount; i++) {
          removeReference((Obj*)closure->upvalues[i]);
        }

        FREE_ARRAY(ObjClosure *, closure->upvalues, closure->upvalueCount);
        FREE(ObjClosure, object);
        break;
    }
    case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        if (function->name != NULL) {
          removeReference((Obj*)function->name);
        }
        freeChunk(&function->chunk);
        FREE(ObjFunction, function);
        break;
    }
    case OBJ_NATIVE:
        FREE(ObjNative, object);
        break;
    case OBJ_STRING: {
        const ObjString *string = (ObjString *)object;
        tableDelete(&vm.strings, OBJ_VAL(string));
        reallocate(object, sizeof(ObjString) + string->length + 1, 0);
        break;
    }
    case OBJ_UPVALUE: {
        ObjUpvalue* upvalue = (ObjUpvalue*)object;
        removeReference((Obj*)upvalue->next);
        removeValueReference(&upvalue->closed);
        FREE(ObjUpvalue, object);
        break;
    }
  }
}

void addReference(Obj *obj) {
#ifdef DEBUG_LOG_GC
  printf("add referenc to %d\n", objType(obj));
#endif // D
  obj->referenceCount++;
}

void addValueReference(Value *value){
  if(IS_OBJ(*value)){
    addReference(AS_OBJ(*value));
  }
}

void removeReference(Obj* obj) {
#ifdef DEBUG_LOG_GC
  printf("remove referenc from %d new ref count %i\n", objType(obj), obj->referenceCount - 1);
#endif
  obj->referenceCount--;

  if(obj->referenceCount == 0){
#ifdef DEBUG_LOG_GC
  printf("referenc from %d is zero. Freeing object. \n", objType(obj));
#endif
    // TODO: remove from the linked list somehow.
    freeObject(obj);
  }
}

void removeValueReference(Value* value) {
  if(IS_OBJ(*value)){
    removeReference(AS_OBJ(*value));
  }
}

void freeObjects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = nextObj(object);
    freeObject(object);
    object = next;
  }
}