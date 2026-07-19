#include "audio_engine.h"
#include "../../arm/arm.h"
#include "Src/assert.h"
#include "Src/def.h"
#include "Src/devices/dac/dac.h"
#include "Src/peripherals/dma/dma.h"
#include "Src/peripherals/rcc/rcc.h"
#include "Src/services/print/printf.h"
#include "Src/peripherals/spi/i2s.h"
#include <stddef.h>
#include <stdint.h>

#define BLOCK_QUEUE_CAPACITY ((uint8_t)8)
static const uint8_t BLOCK_QUEUE_MOD_VAL = BLOCK_QUEUE_CAPACITY - 1;

static audio_engine_t audio_engine_obj;
static block_t SILENCE_BLOCK;
static block_t DUMP_BLOCK;
static block_t block_arr[8];
#define SINE_WAVE_CAP (48)

const int16_t sine_wave[SINE_WAVE_CAP] = {
0, 1045, 2079, 3090, 4067, 5000, 5877, 6691, 
7431, 8089, 8660, 9135, 9510, 9781, 9945, 10000, 
9945, 9781, 9510, 9135, 8660, 8089, 7431, 6691, 
5877, 5000, 4067, 3090, 2079, 1045, 0, -1045, 
-2079, -3090, -4067, -5000, -5877, -6691, -7431, -8089, 
-8660, -9135, -9510, -9781, -9945, -10000, -9945, -9781
};

static void audio_engine_prime(audio_engine_t* self){
    block_t* temp_block_ptr = NULL;
    
    temp_block_ptr = block_queue_dequeue(self->empty_block_queue);
    BARE_ASSERT(temp_block_ptr != NULL);
    self->curr_rx_block = temp_block_ptr;
    
    temp_block_ptr = block_queue_dequeue(self->empty_block_queue);
    BARE_ASSERT(temp_block_ptr != NULL);
    self->next_rx_block = temp_block_ptr;

    temp_block_ptr = block_queue_dequeue(self->empty_block_queue);
    BARE_ASSERT(temp_block_ptr != NULL);
    self->curr_tx_block = temp_block_ptr;

    temp_block_ptr = block_queue_dequeue(self->empty_block_queue);
    BARE_ASSERT(temp_block_ptr != NULL);
    self->next_tx_block = temp_block_ptr;
}

static void audio_engine_prime_test_blocks(audio_engine_t* self){
    uint8_t index = 0;

    for(int i=0; i<BLOCK_QUEUE_CAPACITY; i++){
        for(int j=0; j<BLOCK_DATA_SIZE; j++){
            self->block_arr[i]->data[j] = sine_wave[index%SINE_WAVE_CAP];
            index++;
        }
    }
}

audio_engine_t* audio_engine_init(engine_mode_t mode, RCC_t* rcc){
    for(int i=0; i<BLOCK_QUEUE_CAPACITY; i++){
        audio_engine_obj.block_arr[i] = &block_arr[i];
    }
    audio_engine_obj.empty_block_queue = block_queue_init(QUEUE_TYPE_EMPTY);
    audio_engine_obj.processed_block_queue = block_queue_init(QUEUE_TYPE_PROCESSED);
    audio_engine_obj.raw_block_queue = block_queue_init(QUEUE_TYPE_RAW);
    for(int i=0; i<BLOCK_QUEUE_CAPACITY; i++){
        *audio_engine_obj.block_arr[i] = (block_t){0};
        audio_engine_obj.block_arr[i]->state = BLOCK_STATE_EMPTY;
        block_queue_enqueue(
            audio_engine_obj.empty_block_queue, 
            audio_engine_obj.block_arr[i]
        );
    }
    audio_engine_obj.consequetive_underrun = 0;
    audio_engine_obj.mode = mode;
    // audio_engine_obj.state = ENGINE_STATE_IDLE;
    SILENCE_BLOCK = (block_t){0};
    SILENCE_BLOCK.state = BLOCK_STATE_SILENCE;
    DUMP_BLOCK = (block_t){0};
    DUMP_BLOCK.state = BLOCK_STATE_DUMP;

    if(mode == ENGINE_MODE_NORMAL){
        audio_engine_prime(&audio_engine_obj);

        I2S_handle_t* i2s_handle_tx = i2s_configure(
            I2S_INSTANCE_3,
            rcc, 
            I2S_MODE_DMA_TX, 
            NULL, 
            (DMA_callback_t)&audio_engine_tx_dma_TC_callback,
            NULL,
            (void *)&audio_engine_obj,
            (uint32_t)audio_engine_obj.curr_tx_block->data, 
            (uint32_t)audio_engine_obj.next_tx_block->data
        );

        I2S_handle_t* i2s_handle_rx = i2s_configure(
            I2S_INSTANCE_2,
            rcc, 
            I2S_MODE_DMA_RX, 
            NULL, 
            (DMA_callback_t)&audio_engine_rx_dma_TC_callback,
            NULL,
            (void *)&audio_engine_obj,
            (uint32_t)audio_engine_obj.curr_rx_block->data, 
            (uint32_t)audio_engine_obj.next_rx_block->data
        );

        i2s_init(i2s_handle_rx);
        i2s_init(i2s_handle_tx);
        dac_init(rcc);
    }
    else if(mode == ENGINE_MODE_TESTING){
        audio_engine_prime_test_blocks(&audio_engine_obj);
        audio_engine_prime(&audio_engine_obj);

        for(int i=0; i<3; i++){
            block_queue_enqueue(
                audio_engine_obj.processed_block_queue, 
                block_queue_dequeue(audio_engine_obj.empty_block_queue)
            );
        }

        I2S_handle_t* i2s_handle_tx = i2s_configure(
            I2S_INSTANCE_3,
            rcc, 
            I2S_MODE_DMA_TX, 
            NULL, 
            (DMA_callback_t)&audio_engine_tx_dma_TC_callback,
            NULL,
            (void *)&audio_engine_obj,
            (uint32_t)audio_engine_obj.curr_tx_block->data, 
            (uint32_t)audio_engine_obj.next_tx_block->data
        );

        i2s_init(i2s_handle_tx);
        dac_init(rcc);
    }
    else{
        __BKPT(0);
        // kendrick didn't go far enough!
        // what are you doing!
    }

    return &audio_engine_obj;    
}


static block_t* empty_queue_buffer[BLOCK_QUEUE_CAPACITY];
static block_t* raw_queue_buffer[BLOCK_QUEUE_CAPACITY];
static block_t* processed_queue_buffer[BLOCK_QUEUE_CAPACITY];

static block_queue_t empty_queue_obj;
static block_queue_t raw_queue_obj;
static block_queue_t processed_queue_obj;

block_queue_t* block_queue_init(block_queue_type_t type){
    block_queue_t* block_queue_ptr = NULL;
    switch(type){
        case QUEUE_TYPE_EMPTY:
            block_queue_ptr = &empty_queue_obj;
            block_queue_ptr->buffer = empty_queue_buffer;
            block_queue_ptr->capacity = BLOCK_QUEUE_CAPACITY;
            block_queue_ptr->head = 0;
            block_queue_ptr->tail = 0;
            break;
        case QUEUE_TYPE_RAW:
            block_queue_ptr = &raw_queue_obj;
            block_queue_ptr->buffer = raw_queue_buffer;
            block_queue_ptr->capacity = BLOCK_QUEUE_CAPACITY;
            block_queue_ptr->head = 0;
            block_queue_ptr->tail = 0;
            break;
        case QUEUE_TYPE_PROCESSED:
            block_queue_ptr = &processed_queue_obj;
            block_queue_ptr->buffer = processed_queue_buffer;
            block_queue_ptr->capacity = BLOCK_QUEUE_CAPACITY;
            block_queue_ptr->head = 0;
            block_queue_ptr->tail = 0;
            break;
        default:
            __BKPT(0);
            // don't mess around with my enums
    }
    return block_queue_ptr;
}

__INLINE __bool block_queue_enqueue(block_queue_t* self, block_t* data){
    if(block_queue_is_full(self) == TRUE){
        return FALSE;
    }
    self->buffer[(self->head & BLOCK_QUEUE_MOD_VAL)] = data; //set the data
    __DMB();
    self->head++;
    return TRUE;
}

__INLINE block_t* block_queue_dequeue(block_queue_t* self){
    if(block_queue_is_empty(self) == TRUE){
        return NULL;
    }
    block_t* data = self->buffer[(self->tail & BLOCK_QUEUE_MOD_VAL)];
    __DMB();
    self->tail++;
    return data;
}

__INLINE __bool block_queue_is_empty(block_queue_t* self){
    if(self->head == self->tail){
        return TRUE;
    }
    return FALSE;
}

__INLINE __bool block_queue_is_full(block_queue_t* self){
    if((self->head - self->tail) == self->capacity){
        return TRUE;
    }
    return FALSE;
}

uint8_t block_queue_get_size(block_queue_t* self){
    return (self->head - self->tail);
}


// TODO: CHANGE THE CALLBACK'S PARAMS TO DMA HANDLE
__INLINE void audio_engine_tx_dma_TC_callback(DMA_handle_t* dma_handle){
    // I have completed te transfer and after this callback we will head to the new buffer
    // I think here one thing I need to do is get the next block that I can use, 
    audio_engine_t* self = (audio_engine_t*)dma_handle->user_data;
    block_t* temp_block_ptr = NULL;
    __bool result = FALSE;
    // setting curr block to empty queue
    if(self->curr_tx_block != &SILENCE_BLOCK){
        self->curr_tx_block->state = BLOCK_STATE_EMPTY;
        result = block_queue_enqueue(self->empty_block_queue, self->curr_tx_block);
        if(result != TRUE){
            __BKPT(0);
            // there are only 8 blocks total which is also the cap for queue
            // whatever happended it is just too far gone now. 
            // crash
        }
    }
    // setting next block to curr
    if(self->next_tx_block != &SILENCE_BLOCK){
        self->next_tx_block->state = BLOCK_STATE_READING;
    }
    self->curr_tx_block = self->next_tx_block;

    // getting processed block from queue to next block
    temp_block_ptr = block_queue_dequeue(self->processed_block_queue);
    if(temp_block_ptr == NULL){
        temp_block_ptr = &SILENCE_BLOCK;
        self->consequetive_underrun++;
    }else{
        self->consequetive_underrun = 0;
    }
    
    DMA_set_next_buffer(dma_handle->driver, dma_handle->stream, (uint32_t)temp_block_ptr->data);
    self->next_tx_block = temp_block_ptr;
    __DMB();
    if(self->mode == ENGINE_MODE_TESTING){
        audio_engine_rx_dma_TC_callback(dma_handle);
    }
}

// TODO: CHANGE THE CALLBACK'S PARAMS TO DMA HANDLE
__INLINE void audio_engine_rx_dma_TC_callback(DMA_handle_t* dma_handle){
    audio_engine_t* self = (audio_engine_t*)dma_handle->user_data;
    __bool result = FALSE;
    block_t* temp_block_ptr = NULL;

    // setting current to raw queue
    if(self->curr_rx_block != &DUMP_BLOCK){
        self->curr_rx_block->state = BLOCK_STATE_RAW;
        result = block_queue_enqueue(self->raw_block_queue, self->curr_rx_block);;
        if(result == FALSE){
            __BKPT(0);
            // this should never happen
            // cap should never be hit something is wrong. 
        }
    }   

    // setting next to current block
    if(self->next_rx_block != &DUMP_BLOCK){
        self->next_rx_block->state = BLOCK_STATE_WRITING;   
    }
    self->curr_rx_block = self->next_rx_block;

    // setting empty block to next block
    temp_block_ptr = block_queue_dequeue(self->empty_block_queue);
    if(temp_block_ptr == NULL){
        temp_block_ptr = &DUMP_BLOCK;
        self->consequetive_overrun++;
        // we are moving faster than the speed of consuming
        // or processing
    }else{
        self->consequetive_overrun = 0;
    }

    DMA_set_next_buffer(dma_handle->driver, dma_handle->stream, (uint32_t)temp_block_ptr->data);
    self->next_rx_block = temp_block_ptr;
    __DMB();
}

__STATIC_INLINE void audio_engine_handle_events(audio_engine_t* self){
    // 1. threshold
    block_t* temp_block_ptr = NULL;
    __bool result = FALSE;
    uint32_t irq_state = __get_PRIMASK();
    if(self->consequetive_overrun >= 1){
        // we are out of empty blocks
        // we take one of raw blocks and add it to the
        temp_block_ptr = block_queue_dequeue(self->raw_block_queue);
        if(temp_block_ptr == NULL){
            // we take the processed queue's block and add it to the empty;
            // this is a possible race condition but becase it is so rare
            __disable_irq();
            __ISB();
            temp_block_ptr = block_queue_dequeue(self->processed_block_queue);
            __set_PRIMASK(irq_state);
            __ISB();
            if(temp_block_ptr == NULL){
                __BKPT(0);
                // everything is empty Idk what is going on but all the blocks
                // are outside of the queue which shouldn't have happened
            }
        }
        BARE_ASSERT(temp_block_ptr != &DUMP_BLOCK);

        __disable_irq();
        __ISB();
        result = block_queue_enqueue(self->empty_block_queue, temp_block_ptr);
        self->consequetive_overrun = 0;
        __set_PRIMASK(irq_state);
        __ISB();
        if(result == FALSE){
            __BKPT(0);
            // again shouldn't happen ever
        }
    }
    if(self->consequetive_underrun >= 1){
        // we are out of processed blocks
        // we actually don't need to do anything here
        // we already handled it in the tx isr
    }
}

#define SILENCE_THRESHOLD ((int16_t)0x000F) 
/*
    retuns true if we did find a silence block
    return block if the block is not silent
*/
 __STATIC_INLINE __bool audio_engine_silence_check(const block_t* block){
    uint16_t i=0; 
    while(i < BLOCK_DATA_SIZE){
        if((block->data[i] > SILENCE_THRESHOLD) || 
        (block->data[i] < -SILENCE_THRESHOLD))
        {
            return FALSE;
        }
        i++;
    }
    return TRUE;
}

void audio_engine_processing(audio_engine_t* self){
    audio_engine_handle_events(self);

    // get the next block from memory
    block_t* raw_block_ptr = NULL;
    __bool result = FALSE;
    raw_block_ptr = block_queue_dequeue(self->raw_block_queue);
    if(raw_block_ptr == NULL){
        // there is no work to do
        // lets go to the mall today
        return;
    }
    BARE_ASSERT(raw_block_ptr != &DUMP_BLOCK);

    raw_block_ptr->state = BLOCK_STATE_PROCESSED;
    result = block_queue_enqueue(self->processed_block_queue, raw_block_ptr);
    if(result == FALSE){
        __BKPT(0);
        // what is going on here why is my dma tx not processing shit?
    }
}   