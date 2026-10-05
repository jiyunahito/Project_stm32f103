#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "main.h"

#define POINT_NUM 256
#define X_FRAME_MIN 20
#define X_FRAME_MAX 220
#define Y_FRAME_MIN 10
#define Y_FRAME_MAX 310
#define X_FRAME_CENTER ((X_FRAME_MIN + X_FRAME_MAX) / 2)
#define BACKGROUND_COLOR BLACK

extern const uint16_t uspectrumSin_Table[3];

void vspectrumDrawGrid(void);
void vspectrumDAC_DMA_Init(void);
void vspectrumFrequency_Change(uint32_t PSC, uint32_t ARR);
void vspectrumUpdate_Waveform(const uint16_t *wavedata);

#endif