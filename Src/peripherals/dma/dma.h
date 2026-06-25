#ifndef DMA_H
#define DMA_H

#include "Src/def.h"
#include "Src/peripherals/rcc/rcc.h"
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
    DMA_CIRC_DIS = 0x0,
    DMA_CIRC_EN = 0x1
}DMA_circ_mode_t;

typedef enum DMA_db_mode{
    DMA_DB_DIS = 0x0,
    DMA_DB_EN = 0x1
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

typedef enum DMA_mode{
    DMA_MODE_DIRECT = 0x0,
    DMA_MODE_FIFO = 0x1
}DMA_mode_t;

typedef enum DMA_FIFO_threshold{
    DMA_FIFO_1_4_FULL = 0x0,
    DMA_FIFO_1_2_FULL = 0x1,
    DMA_FIFO_3_4_FULL = 0x2,
    DMA_FIFO_FULL = 0x3
}DMA_FIFO_threshold_t;

typedef enum DMA_instance{
    DMA_INSTANCE_1 = 0x0,
    DMA_INSTANCE_2 = 0x1
}DMA_instance_t;

typedef struct DMA_config{
    DMA_stream_id_t stream;
    DMA_channel_t channel;
    DMA_incr_t memory_bust;
    DMA_incr_t peripheral_burst;
    DMA_target_t curr_target;
    DMA_db_mode_t double_buffer_mode;
    DMA_priority_t priority_level;
    DMA_pincos_t peripheral_incrmnt_offset;
    DMA_mem_size_t memory_data_size;
    DMA_mem_size_t peripheral_data_size;
    DMA_minc_t mem_incr_mode;
    DMA_pinc_t periph_incr_mode;
    DMA_circ_mode_t circ_mode;
    DMA_dir_t data_direction;
    DMA_pfctrl_t periph_flow_crtl;
    __bool TC_intrpt_en;
    __bool HT_intrpt_en;
    __bool TE_intrpt_en;
    __bool DME_intrpt_en;
    uint16_t no_of_items;
    uint32_t peripheral_addr;
    uint32_t mem0_addr;
    uint32_t mem1_addr;
    DMA_mode_t mode;
    __bool FIFO_err_intrpt_en;
    DMA_FIFO_threshold_t fifo_threshold;
}DMA_config_t;

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

DMA_driver_t* DMA_init(DMA_config_t* config, DMA_instance_t instance,RCC_t* rcc);

// void DMA_set_chsel(DMA_driver_t* self, DMA_stream_id_t stream_id, 
// DMA_channel_t channel);

// void DMA_set_mbrust(DMA_driver_t* self, DMA_stream_id_t stream_id, 
// DMA_incr_t incr);

// void DMA_set_pburst(DMA_driver_t* self, DMA_stream_id_t stream_id, 
// DMA_incr_t incr);

// void DMA_set_curr_target(DMA_driver_t* self, DMA_stream_id_t stream_id, 
// DMA_target_t target);

// void DMA_set_dbm(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_db_mode_t mode);

// void DMA_set_priority(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_priority_t priority);

// void DMA_set_pincos(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_pincos_t offset_size);

// void DMA_set_msize(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_mem_size_t mem_size);

// void DMA_set_psize(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_mem_size_t mem_size);

// void DMA_set_minc(DMA_driver_t* self, DMA_stream_id_t stream_id,
// DMA_minc_t mem_inc);

// void DMA_set_pinc(DMA_driver_t* self, DMA_stream_id_t stream_id, 
// DMA_pinc_t periph_inc);

// void DMA_set_circ(DMA_driver_t* seld, DMA_stream_id_t stream_id, 
// DMA_circ_mode_t circ_mode);

// void DMA_set_dir(DMA_driver_t* seld, DMA_stream_id_t stream_id, 
// DMA_dir_t dir);

// void DMA_set_pfctrl(DMA_driver_t* seld, DMA_stream_id_t stream_id, 
// DMA_pfctrl_t flow);

/**
okay here is the idea that I have at the moment, 
for dma's SxCR 
I can create helper functions for things that I would need to change
but I think because all the config would have to be in a state where
en bit = 0
I think a better way to approach this would be to take that setting
set it all in once, 
and then do a final write, 
this can make this so much faster and efficient than what I have planned
*/


#endif