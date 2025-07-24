#ifndef clox_line_h
#define clox_line_h

typedef struct {
    int count;
    int capacity;
    int* lines;
    int* length;
} LineArray;

void initLineArray(LineArray* array);
void freeLineArray(LineArray* array);
void writeLineArray(LineArray* array, int line);

int getLine(const LineArray* array, int offset);

#endif //clox_line_h
