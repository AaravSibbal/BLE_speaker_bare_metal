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

typedef enum DMA_channel{
    DMA_CHANNEL_0 = 0x0,
    DMA_CHANNEL_1 = 0x1,
    DMA_CHANNEL_2 = 0x2,
    DMA_CHANNEL_3 = 0x3,
    DMA_CHANNEL_4 = 0x4,
    DMA_CHANNEL_5 = 0x5,
    DMA_CHANNEL_6 = 0x6,
    DMA_CHANNEL_7 = 0x7
} DMA_channel_t;

typedef enum DMA_incr{
    DMA_INCR_SINGLE = 0x0,
    DMA_INCR_4 = 0x1,
    DMA_INCR_8 = 0x2,
    DMA_INCR_16 = 0x3
}DMA_incr_t;

typedef enum DMA_target{
    DMA_TARGET_MEM_0 = 0x0,
    DMA_TARGET_MEM_1 = 0x1
}DMA_target_t;


typedef enum DMA_priority{
    DMA_PRIORITY_LOW = 0x0,
    DMA_PRIORITY_MEDIUM = 0x1,
    DMA_PRIORITY_HIGH = 0x2,
    DMA_PRIORITY_VERY_HIGH = 0x3
}DMA_priority_t;

typedef enum DMA_mem_size{
    DMA_MEM_8_BIT = 0x0,
    DMA_MEM_16_BIT = 0x1,
    DMA_MEM_32_BIT = 0x2
}DMA_mem_size_t;

typedef enum DMA_minc{
    DMA_MINC_FIXED = 0x0,
    DMA_MINC_INCREMENT = 0x1
}DMA_minc_t;

typedef enum DMA_circ_mode{
    DMA_CIRC_EN = 0x0,
    DMA_CIRC_DIS = 0x1,
}

typedef enum DMA_db_mode{
    DMA_DB_EN = 0x0, 
    DMA_DB_DIS = 0x1
}DMA_db_mode_t;

typedef enum DMA_pinc{
    DMA_PINC_FIXED = 0x0,
    DMA_PINC_INCREMENT = 0x1
}DMA_pinc_t;

typedef enum DMA_pincos{
    DMA_PINCOS_LINKED_TO_PSIZE = 0x0,
    DMA_PINCOS_FIXES_32_BIT = 0x1
}DMA_pincos_t;

typedef enum DMA_dir{
    DMA_DIR_PERIPH_TO_MEM = 0x0,
    DMA_DIR_MEM_TO_PERIPH = 0x1,
    DMA_DIR_MEM_TO_MEM = 0x2
}DMA_dir_t;

typedef enum DMA_pfcrtl{
    DMA_CTRL_DMA = 0x0,
    DMA_CTRL_PERIPH = 0x1
}DMA_pfctrl_t;

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

void DMA_en_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_en_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_en_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_en_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);

void DMA_dis_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_dis_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_dis_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_dis_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id);

void DMA_en_stream(DMA_driver_t* self, DMA_stream_id_t stream_id);
void DMA_dis_stream(DMA_driver_t* self, DMA_stream_id_t stream_id);

void DMA_set_chsel(DMA_driver_t* self, DMA_stream_id_t stream_id, 
DMA_channel_t channel);

void DMA_set_mbrust(DMA_driver_t* self, DMA_stream_id_t stream_id, 
DMA_incr_t incr);

void DMA_set_pburst(DMA_driver_t* self, DMA_stream_id_t stream_id, 
DMA_incr_t incr);

void DMA_set_curr_target(DMA_driver_t* self, DMA_stream_id_t stream_id, 
DMA_target_t target);


#endif