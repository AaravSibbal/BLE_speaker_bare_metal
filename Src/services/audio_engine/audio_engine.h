#ifndef AUDIO_ENGINE
#define AUDIO_ENGINE

#include "Src/peripherals/dma/dma.h"
#include "stdint.h"


typedef enum block_state{
    BLOCK_STATE_EMPTY = 0,
    BLOCK_STATE_READING = 1,
    BLOCK_STATE_PROCESSING = 2,
    BLOCK_STATE_PROCESSED = 3,
    BLOCK_STATE_WRITING = 4,
    BLOCK_STATE_RAW = 5,
    BLOCK_STATE_SILENCE = 6,
    BLOCK_STATE_DUMP = 7
}block_state_t;

typedef struct block{
    uint16_t data[2048];
    volatile block_state_t state;
}block_t;

typedef struct block_queue{
    block_t** buffer;
    volatile uint8_t head;
    volatile uint8_t tail;
    uint8_t capacity; //defaults to 8    
}block_queue_t;

typedef enum block_queue_type{
    QUEUE_TYPE_RAW = 0,
    QUEUE_TYPE_EMPTY = 1,
    QUEUE_TYPE_PROCESSED = 2
}block_queue_type_t;

block_queue_t* block_queue_init(block_queue_type_t type);
__bool block_queue_enqueue(block_queue_t* self, block_t* data);//return true if we can enqueue
block_t* block_queue_dequeue(block_queue_t* self); // will be null if can't do it
__bool block_queue_is_empty(block_queue_t* self); //true if it is empty
__bool block_queue_is_full(block_queue_t* self);//true if it is full

typedef enum engine_state{
    ENGINE_STATE_IDLE = 0, // the dac is off, dma tx is off
    ENGINE_STATE_WAKE_UP = 1, // we are recieving data now,but we need more proof
    ENGINE_STATE_RUNNING = 2 // we also got the proof we are runnning the engine full swing
}engine_state_t;

// I need to know the index of next processed block

typedef struct audio_engine{
    block_t* block_arr[8];
    block_queue_t* empty_block_queue;
    block_queue_t* processed_block_queue;
    block_queue_t* raw_block_queue;
    block_t* curr_tx_block;
    block_t* next_tx_block;
    block_t* curr_rx_block;
    block_t* next_rx_block;
    volatile engine_state_t state;
    volatile uint32_t consequetive_underrun;
    volatile uint32_t consequetive_overrun;
}audio_engine_t;

block_t* block_init(uint8_t capacity);
audio_engine_t* audio_engine_init(void);

// I need to think what will happen when state is wake up:

void audio_engine_tx_dma_TC_callback(audio_engine_t* self, DMA_driver_t* driver, DMA_stream_id_t stream);
void audio_engine_rx_dma_TC_callback(audio_engine_t* self, DMA_driver_t* driver, DMA_stream_id_t stream);

void audio_engine_underrun_event(audio_engine_t* self);
void audio_engine_overrun_event(audio_engine_t* self);

// empty  empty  
// [0]   [1]     [2]     [3]      [4]   [5]          [6]      [7]   [8]
// tx1    tx2    empty   empty    empty process      process  rx1   rx2   
#endif