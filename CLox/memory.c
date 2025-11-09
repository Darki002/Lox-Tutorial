#include "memory.h"
#include <stdlib.h>

#include "object.h"
#include "value.h"
#include "vm.h"
#include "compiler.h"
#include "table.h"

#ifdef DEBUG_LOG_GC
#include "debug.h"
#include <stdio.h>

#endif // DEBUG_LOG_GC

/*
 oldSize | newSize | Operation
 0 | Non-zero | Allocate new block
 Non-zero | 0 | Free allocation
 Non-Zero | Smaller than oldSize | Shrink existing allocation
 Non-zero | Larger than oldSize | Grow existing allocation.
 */
void *reallocate(void *pointer, size_t oldSize, const size_t newSize) {
  if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
    collectGarbage();
#endif // DEBUG_STRESS_GC
  }

  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void *result = realloc(pointer, newSize);
  if (result == NULL)
    exit(1);
  return result;
}

void markObject(Obj *object) {
  if (object == NULL) return;
  if(object->isMarked) return;
  object->isMarked = true;

  if(vm.grayCapacity < vm.grayCount + 1) {
    vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
    vm.grayStack = (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);

    if(vm.grayStack == NULL) exit(1);
  }

  vm.grayStack[vm.grayCount++] = object;

#ifdef DEBUG_LOG_GC
  printf("%p mark ", (void *)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif // DEBUG_LOG_GC
}

void markValue(Value value) {
  if (IS_OBJ(value))
    markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}

static void blackenobject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif // DEBUG_LOG_GC

    switch (object->type) {
        case OBJ_CLOSURE:
            ObjClosure* closure = (ObjClosure*)object;
            markObject((Obj*)closure->function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*)closure->upvalues[i]);
            }
            break;
        case OBJ_FUNCTION:
            ObjFunction* function = (ObjFunction*)object;
            markObject((Obj*)function->name);
            markArray(&function->chunk.constants);
            break;
        case OBJ_UPVALUE:
            markValue(((ObjUpvalue*)object)->closed);
            break;
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}

static void freeObject(Obj *object) {
#ifdef DEBUG_LOG_GC
  printf("%p free type %d\n", (void *)object, object->type);
#endif // DEBUG_LOG_GC

  switch (object->type) {
    case OBJ_CLOSURE: {
        const ObjClosure *closure = (ObjClosure *)object;
        FREE_ARRAY(ObjClosure *, closure->upvalues, closure->upvalueCount);
        FREE(ObjClosure, object);
        break;
    }
    case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        freeChunk(&function->chunk);
        FREE(ObjFunction, function);
        break;
    }
    case OBJ_NATIVE:
        FREE(ObjNative, object);
        break;
    case OBJ_STRING: {
        const ObjString *string = (ObjString *)object;
        reallocate(object, sizeof(ObjString) + string->length + 1, 0);
        break;
    }
    case OBJ_UPVALUE:
        FREE(ObjUpvalue, object);
        break;
  }
}

static void markRoots() {
  for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
    markValue(*slot);
  }

  for (int i = 0; i < vm.globals.count; i++) {
    markValue(vm.globals.values[i].value);
  }

  for (int i = 0; i < vm.frameCount; i++) {
    markObject((Obj *)vm.frames[i].closure);
  }

  ObjUpvalue *upvalue = vm.openUpvalues;
  while (upvalue != NULL) {
    markObject((Obj *)upvalue);
    upvalue = upvalue->next;
  }

  markCompilerRoots();
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenobject(object);
    }
}

static void sweep() {
    Obj* previous = NULL;
    Obj* current = vm.objects;

    while (current != NULL) {
        if(current->isMarked){
            current->isMarked = false;
            previous = current;
            current = current->next;
        } else {
            Obj* unreachable = current;
            current = current->next;

            if(previous != NULL){
                previous->next = current;
            } else {
                vm.objects = current;
            }

            freeObject(unreachable);
        }
    }
}

void collectGarbage() {
#ifdef DEBUG_LOG_GC
  printf("-- gc begin\n");
#endif // DEBUG_LOG_GC

  markRoots();
  traceReferences();
  tableRemoveWhie(&vm.strings);
  sweep();

#ifdef DEBUG_LOG_GC
  printf("-- gc end\n");
#endif // DEBUG_LOG_GC
}

void freeObjects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = object->next;
    freeObject(object);
    object = next;
  }

  free(vm.grayStack);
}