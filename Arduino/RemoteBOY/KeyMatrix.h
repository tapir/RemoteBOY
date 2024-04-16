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
    bool getKeyState(const size_t row, const size_t col);

private:
    bool state[NUM_ROWS][NUM_COLS];
};

#endif // MATRIX_H