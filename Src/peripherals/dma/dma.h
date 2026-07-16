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
    DMA_STREAM_7 = 7,
    DMA_BAD_STREAM = 0x8
}DMA_stream_id_t;

typedef enum DMA_channel{
    DMA_CHANNEL_0 = 0x0,
    DMA_CHANNEL_1 = 0x1,
    DMA_CHANNEL_2 = 0x2,
    DMA_CHANNEL_3 = 0x3,
    DMA_CHANNEL_4 = 0x4,
    DMA_CHANNEL_5 = 0x5,
    DMA_CHANNEL_6 = 0x6,
    DMA_CHANNEL_7 = 0x7,
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

typedef enum DMA_buff_size{
    DMA_BUFF_SIZE_1 = 0,
    DMA_BUFF_SIZE_2 = 1,
    DMA_BUFF_SIZE_4 = 2,
    DMA_BUFF_SIZE_8 = 3,
    DMA_BUFF_SIZE_16 = 4,
    DMA_BUFF_SIZE_32 = 5,
    DMA_BUFF_SIZE_64 = 6,
    DMA_BUFF_SIZE_128 = 7,
    DMA_BUFF_SIZE_256 = 8,
    DMA_BUFF_SIZE_512 = 9,
    DMA_BUFF_SIZE_1024 = 10,
    DMA_BUFF_SIZE_2048 = 11,
    DMA_BUFF_SIZE_4096 = 12,
    DMA_BUFF_SIZE_8192 = 13
}DMA_buff_size_t;

typedef struct DMA_config{
    DMA_stream_id_t stream;
    DMA_channel_t channel;
    DMA_incr_t memory_burst;
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
    DMA_buff_size_t no_of_items;
    uint32_t peripheral_addr;
    uint32_t mem0_addr;
    uint32_t mem1_addr;
    DMA_mode_t mode;
    __bool FIFO_err_intrpt_en;
    DMA_FIFO_threshold_t fifo_threshold;
}DMA_config_t;

typedef enum DMA_error{
    DMA_ERROR_NONE = 0,
    DMA_ERROR_TRANSFER = 1,
    DMA_ERROR_FIFO = 2,
    DMA_ERROR_DIRECT_MODE = 3
}DMA_error_t;

typedef enum DMA_state{
    DMA_STATE_RESET = 0,
    DMA_STATE_READY = 1,
    DMA_STATE_BUSY = 2,
    DMA_STATE_ERROR = 4
}DMA_state_t;


DMA_driver_t* DMA_configure(DMA_config_t* config, DMA_instance_t instance, RCC_t* rcc);
void DMA_init(DMA_driver_t* driver, DMA_stream_id_t stream);

//for double buffering mode only
void DMA_set_next_buffer(DMA_driver_t* driver, DMA_stream_id_t stream, uint32_t val); 

void DMA1_Stream1_IRQHandler(void);
void DMA1_Stream2_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void DMA1_Stream4_IRQHandler(void);
void DMA1_Stream5_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void);


typedef struct DMA_handle DMA_handle_t;

typedef void (* DMA_callback_t)(DMA_handle_t* handle);
struct DMA_handle{
    DMA_driver_t* driver;
    volatile DMA_error_t error_state;
    volatile DMA_state_t transfer_state;
    DMA_stream_id_t stream;
    DMA_callback_t HC_callback;
    DMA_callback_t TC_callback;
    DMA_callback_t error_callback;
    void *user_data;
};

typedef struct DMA_hndl_config{
    DMA_driver_t* driver;
    DMA_stream_id_t stream;
    DMA_callback_t HC_callback;
    DMA_callback_t TC_callback;
    DMA_callback_t error_callback;
    void *user_data;
}DMA_hndl_config_t;

DMA_handle_t* DMA_handle_init(DMA_hndl_config_t* config);

#endif