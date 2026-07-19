#ifndef AUDIO_ENGINE
#define AUDIO_ENGINE

#include "Src/peripherals/dma/dma.h"
#include "Src/peripherals/rcc/rcc.h"
#include "stdint.h"

typedef enum engine_mode{
    ENGINE_MODE_NORMAL = 0,
    ENGINE_MODE_TESTING = 1
}engine_mode_t;

typedef struct audio_engine audio_engine_t;
typedef struct block block_t;

audio_engine_t* audio_engine_init(engine_mode_t mode, RCC_t* rcc);

void audio_engine_tx_dma_TC_callback(DMA_handle_t* dma_handle);
void audio_engine_rx_dma_TC_callback(DMA_handle_t* dma_handle);
void audio_engine_processing(audio_engine_t* self);

#endif