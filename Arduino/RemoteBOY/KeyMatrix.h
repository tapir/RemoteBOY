#ifndef KEYMATRIX_H
#define KEYMATRIX_H

#include <stddef.h>

static const size_t NUM_ROWS = 5;
static const size_t NUM_COLS = 2;

class KeyMatrix {
public:
    KeyMatrix(void);
    void setup(void);
    void loop(void);
    void reset(void);
    int  getKeyState(size_t row, size_t col);

private:
    int state[NUM_ROWS][NUM_COLS];
};

#endif // MATRIX_H