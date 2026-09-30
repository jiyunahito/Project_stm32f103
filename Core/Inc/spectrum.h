#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "main.h"

#define POINT_NUM 256
#define X_FRAME_MIN 20
#define X_FRAME_MAX 220
#define Y_FRAME_MIN 10
#define Y_FRAME_MAX 310
#define X_FRAME_CENTER ((X_FRAME_MIN + X_FRAME_MAX) / 2)

void vspectrumDrawGrid(void);

#endif