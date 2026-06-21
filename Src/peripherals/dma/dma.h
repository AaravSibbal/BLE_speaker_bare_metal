#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef struct DMA_driver DMA_driver_t;


typedef enum DMA_stream_id{
    DMA_STREAM_0 = 0,
    DMA_STREAM_1 = 1,
    DMA_STREAM_2 = 2,
    DMA_STREAM_3 = 3,
    DMA_STREAM_4 = 4,
    DMA_STREAM_5 = 5,
    DMA_STREAM_6 = 6,
    DMA_STREAM_7 = 7
}DMA_stream_id_t;

/**
    tc: Transfer complete flag
    ht: Half transfer flag
    te: Transfer error flag
    dme: direct mode error flag
    fe: FIFO error flag
    clear flags are all handled in IFCR
*/

void DMA_clear_tc(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_clear_ht(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_clear_te(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_clear_dme(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_clear_fe(DMA_driver_t* self, DMA_stream_id_t stream_id);

uint32_t DMA_get_tc(DMA_driver_t* self, DMA_stream_id_t stream_id);
uint32_t DMA_get_ht(DMA_driver_t* self, DMA_stream_id_t stream_id);
uint32_t DMA_get_te(DMA_driver_t* self, DMA_stream_id_t stream_id);
uint32_t DMA_get_dme(DMA_driver_t* self, DMA_stream_id_t stream_id);
uint32_t DMA_get_fe(DMA_driver_t* self, DMA_stream_id_t stream_id);

#endif