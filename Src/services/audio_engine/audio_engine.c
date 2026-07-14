#include "audio_engine.h"
#include "../../arm/arm.h"
#include "Src/def.h"
#include "Src/peripherals/dma/dma.h"
#include <stddef.h>
#include <stdint.h>

#define BLOCK_QUEUE_CAPACITY ((uint8_t)8)
static const uint8_t BLOCK_QUEUE_MOD_VAL = BLOCK_QUEUE_CAPACITY - 1;

static audio_engine_t audio_engine_obj;
static const block_t SILENCE_BLOCK;
static block_t DUMP_BLOCK;

audio_engine_t* audio_engine_init(){
    for(int i=0; i<BLOCK_QUEUE_CAPACITY; i++){
        *audio_engine_obj.block_arr[i] = (block_t){ 0 };
    }
    audio_engine_obj.empty_block_queue = block_queue_init(QUEUE_TYPE_EMPTY);
    audio_engine_obj.processed_block_queue = block_queue_init(QUEUE_TYPE_PROCESSED);
    audio_engine_obj.raw_block_queue = block_queue_init(QUEUE_TYPE_RAW);
    for(int i=0; i<BLOCK_QUEUE_CAPACITY; i++){
        audio_engine_obj.block_arr[i]->state = BLOCK_STATE_EMPTY;
        block_queue_enqueue(
            audio_engine_obj.empty_block_queue, 
            audio_engine_obj.block_arr[i]
        );
    }
    audio_engine_obj.consequetive_underrun = 0;
    audio_engine_obj.state = ENGINE_STATE_IDLE;
    SILENCE_BLOCK.data = {0};
    SILENCE_BLOCK.state = BLOCK_STATE_SILENCE;
    DUMP_BLOCK.data = {0};
    DUMP_BLOCK.state = BLOCK_STATE_DUMP;
    return &audio_engine_obj;    
}

static block_t* empty_queue_buffer[8];
static block_t* raw_queue_buffer[BLOCK_QUEUE_CAPACITY];
static block_t* processed_queue_buffer[BLOCK_QUEUE_CAPACITY];

static block_queue_t empty_queue_obj;
static block_queue_t raw_queue_obj;
static block_queue_t processed_queue_obj;

block_queue_t* block_queue_init(block_queue_type_t type){
    block_queue_t* block_queue_ptr;
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

__bool block_queue_enqueue(block_queue_t* self, block_t* data){
    if(block_queue_is_full(self) == TRUE){
        return FALSE;
    }
    self->buffer[(self->head & BLOCK_QUEUE_MOD_VAL)] = data; //set the data
    __DMB();
    self->head++;
    return TRUE;
}

block_t* block_queue_dequeue(block_queue_t* self){
    if(block_queue_is_empty(self) == TRUE){
        return NULL;
    }
    block_t* data = self->buffer[(self->tail & BLOCK_QUEUE_MOD_VAL)];
    __DMB();
    self->tail++;
    return data;
}

__bool block_queue_is_empty(block_queue_t* self){
    if(self->head == self->tail){
        return TRUE;
    }
    return FALSE;
}

__bool block_queue_is_full(block_queue_t* self){
    if((self->head - self->tail) == self->capacity){
        return TRUE;
    }
    return FALSE;
}

__INLINE void audio_engine_tx_dma_TC_callback(audio_engine_t* self, DMA_driver_t* driver, DMA_stream_id_t stream){
    // I have completed te transfer and after this callback we will head to the new buffer
    // I think here one thing I need to do is get the next block that I can use, 
    
    block_t* temp_block_ptr = NULL;
    __bool result = FALSE;
    switch (self->state) {
        case ENGINE_STATE_IDLE:
            // this shouldn't happen in this state, because by the definition of the state dmatx should be off
            __BKPT(0);
            break;
        case ENGINE_STATE_WAKE_UP:
            // because we are defining the wake_up state as dmatx as off this state is invalid as well
            __BKPT(0);
            break;
        case ENGINE_STATE_RUNNING:
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
            
            DMA_set_next_buffer(driver, stream, (uint32_t)temp_block_ptr->data);
            self->next_tx_block = temp_block_ptr;

            break;
        default:
            __BKPT(0);//don't fuck with my enums!!
    }
    __DMB();
}

void audio_engine_rx_dma_TC_callback(audio_engine_t* self, DMA_driver_t* driver, DMA_stream_id_t stream){
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

    DMA_set_next_buffer(driver, stream, (uint32_t)temp_block_ptr->data);
    self->next_rx_block = temp_block_ptr;
    __DMB();
}
