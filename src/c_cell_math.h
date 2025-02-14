#ifndef C_CELL_MATH_H
#define C_CELL_MATH_H

#include "result.h"

typedef struct ivec2 ivec2;
struct ivec2 {
    union {
        int raw[2];
        struct { int x, y; };
    };
};

DEFINE_RESULT_TYPE(ivec2);

#endif /* C_CELL_MATH_H */
