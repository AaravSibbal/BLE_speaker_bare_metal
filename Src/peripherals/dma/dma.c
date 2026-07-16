#include "dma.h"

#include <stdint.h>
#include "../../def.h"
#include "Src/arm/arm.h"
#include "Src/assert.h"

typedef struct DMA_stream{
    __IO uint32_t CR;
    __IO uint32_t NDTR;
    __IO uint32_t PAR;
    __IO uint32_t M0AR;
    __IO uint32_t M1AR;
    __IO uint32_t FCR;
} DMA_stream_t;

struct DMA_driver{
    __IO uint32_t ISR[2];
    __IO uint32_t IFCR[2];
    DMA_stream_t streams[8];
};


typedef enum intrpt_reg{
    FIFO_ERROR = 0,
    DIRECT_MODE_ERROR = 2,
    TRANSFER_ERROR = 3,
    HALF_TRANSFER = 4,
    TRANSFER_COMPLETE = 5
}intrpt_reg_t;

#define DMA1_BASE_ADDR (0x40026000)
#define DMA2_BASE_ADDR (0x40026400)

static DMA_handle_t dma0_handle;
static DMA_handle_t dma1_handle;
static DMA_handle_t dma2_handle;
static DMA_handle_t dma3_handle;
static DMA_handle_t dma4_handle;
static DMA_handle_t dma5_handle;
static DMA_handle_t dma6_handle;
static DMA_handle_t dma7_handle;

static const uint32_t DMA_CT_START_BIT = 19;

__STATIC_INLINE uint32_t DMA_get_intrpt_bit(DMA_stream_id_t stream_id, intrpt_reg_t reg){
    uint32_t some = stream_id%4;
    switch(some){
        case 0:
            return reg;
        case 1:
            return (reg+6);
        case 2:
            return (reg+16);
        case 3:
            return (reg+22);
        default:
            // something really went wrong cause apparently
            // mod is not working
            __BKPT(0);
    }
    return 0;
}

__STATIC_INLINE void DMA_clear_tc(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_COMPLETE
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);
}

__STATIC_INLINE void DMA_clear_ht(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        HALF_TRANSFER 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__STATIC_INLINE void DMA_clear_te(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__STATIC_INLINE void DMA_clear_dme(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        DIRECT_MODE_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__STATIC_INLINE void DMA_clear_fe(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        FIFO_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__STATIC_INLINE uint8_t DMA_get_tc(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_COMPLETE 
    );
    
    if(self->ISR[ifcr_idx] & (1UL<<clear_bit)){
        return 1;
    }
    return 0;
}

__STATIC_INLINE uint8_t DMA_get_ht(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        HALF_TRANSFER 
    );
    
    if(self->ISR[ifcr_idx] & (1UL<<clear_bit)){
        return 1;
    }
    return 0;
}
__STATIC_INLINE uint8_t DMA_get_te(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_ERROR 
    );
    
    if(self->ISR[ifcr_idx] & (1UL<<clear_bit)){
        return 1;
    }
    return 0;
}
__STATIC_INLINE uint8_t DMA_get_dme(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        DIRECT_MODE_ERROR 
    );
    
    if(self->ISR[ifcr_idx] & (1UL<<clear_bit)){
        return 1;
    }
    return 0;
}
__STATIC_INLINE uint8_t DMA_get_fe(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        FIFO_ERROR 
    );
    
    if(self->ISR[ifcr_idx] & (1UL<<clear_bit)){
        return 1;
    }
    return 0;
}

#define DMA_TCIE_BIT 4UL

__STATIC_INLINE void DMA_en_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
    bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_TCIE_BIT, 
        1
    );
}

#define DMA_HTIE_BIT 3UL

__STATIC_INLINE void DMA_en_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_HTIE_BIT, 
        1
    );   
}

#define DMA_TEIE_BIT 2UL

__STATIC_INLINE void DMA_en_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_TEIE_BIT, 
        1
    );   
}

#define DMA_DMEIE_BIT 1UL

__STATIC_INLINE void DMA_en_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        1
    );   
}


__STATIC_INLINE void DMA_dis_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__STATIC_INLINE void DMA_dis_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__STATIC_INLINE void DMA_dis_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__STATIC_INLINE void DMA_dis_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}

#define DMA_EN_BIT 0UL

__STATIC_INLINE void DMA_en_stream(DMA_driver_t* self, DMA_stream_id_t stream_id){
    bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_EN_BIT, 
        1
    );      
}

__STATIC_INLINE void DMA_dis_stream(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_EN_BIT, 
        0
    );         
}

__STATIC_INLINE DMA_driver_t* DMA_get_instance(DMA_instance_t instance){
    switch (instance) {
        case DMA_INSTANCE_1:
            return (DMA_driver_t*)DMA1_BASE_ADDR;
        case DMA_INSTANCE_2:
            return (DMA_driver_t*)DMA2_BASE_ADDR;
        default:
            return NULL;
    }
}

static const uint8_t DMA_CIRC_START_BIT = 8;
__STATIC_INLINE uint8_t DMA_get_circ_mode(DMA_driver_t* self, DMA_stream_id_t stream){
    return ((self->streams[stream].CR >> DMA_CIRC_START_BIT) & (0x1UL));
}


DMA_driver_t* DMA_configure(DMA_config_t* config, DMA_instance_t instance, RCC_t* rcc){
    static const uint32_t DMA_CR_RESERVED_START_BIT = 28;
    static const uint32_t DMA_CR_RESERVED_FIELD_LEN = 4;
    static const uint32_t DMA_CR_RESERVED1_START_BIT = 20;
    static const uint32_t DMA_CHSEL_START_BIT = 25;
    static const uint32_t DMA_MBURST_START_BIT = 23;
    static const uint32_t DMA_PBURST_START_BIT = 21;
    static const uint32_t DMA_DBM_START_BIT = 18;
    static const uint32_t DMA_PL_START_BIT = 16;
    static const uint32_t DMA_PINCOS_START_BIT = 15;
    static const uint32_t DMA_MSIZE_START_BIT = 13;
    static const uint32_t DMA_PSIZE_START_BIT = 11;
    static const uint32_t DMA_MINC_START_BIT = 10;
    static const uint32_t DMA_PINC_START_BIT = 9;
    static const uint32_t DMA_DIR_START_BIT = 6;
    static const uint32_t DMA_PFCTRL_START_BIT = 5;
    static const uint32_t DMA_FCR_RESERVED_MSK = 
    ((uint32_t)( ( 1<<(32-8) ) -1 )<<8) | (1UL<<6);
    static const uint32_t DMA_DIRECT_MODE_BIT = 2;
    static const uint32_t DMA_FTH_BIT = 0;
    static const uint32_t DMA_FEIE_BIT = 7;

    BARE_ASSERT(config != NULL);
    BARE_ASSERT(rcc != NULL);
    BARE_ASSERT(config->stream != DMA_BAD_STREAM);

    DMA_driver_t *driver = DMA_get_instance(instance);
    BARE_ASSERT(driver != NULL);

    if(instance == DMA_INSTANCE_1){
        RCC_en_DMA1(rcc);
    }else if(instance == DMA_INSTANCE_2){
        RCC_en_DMA2(rcc);
    }else{
        // we fucked up bad
        __BKPT(0);
    }

    DMA_dis_stream(driver, config->stream);
    // make sure that the reserved are preserved and everything is 0'd
    uint32_t DMA_cr = driver->streams[config->stream].CR;
    DMA_cr &= (
        msk_of_ones(DMA_CR_RESERVED_FIELD_LEN) << DMA_CR_RESERVED_START_BIT
        | (1 << DMA_CR_RESERVED1_START_BIT)
    );

    // write the config to the registers
    DMA_cr |= ((uint32_t)config->channel << DMA_CHSEL_START_BIT);
    DMA_cr |= ((uint32_t)config->memory_burst << DMA_MBURST_START_BIT);
    DMA_cr |= ((uint32_t)config->peripheral_burst << DMA_PBURST_START_BIT);
    DMA_cr |= ((uint32_t)config->curr_target << DMA_CT_START_BIT);
    DMA_cr |= ((uint32_t)config->double_buffer_mode << DMA_DBM_START_BIT);
    DMA_cr |= ((uint32_t)config->priority_level << DMA_PL_START_BIT);
    DMA_cr |= ((uint32_t)config->peripheral_incrmnt_offset << DMA_PINCOS_START_BIT);
    DMA_cr |= ((uint32_t)config->memory_data_size << DMA_MSIZE_START_BIT);
    DMA_cr |= ((uint32_t)config->peripheral_data_size << DMA_PSIZE_START_BIT);
    DMA_cr |= ((uint32_t)config->mem_incr_mode << DMA_MINC_START_BIT);
    DMA_cr |= ((uint32_t)config->periph_incr_mode<< DMA_PINC_START_BIT);
    DMA_cr |= ((uint32_t)config->circ_mode << DMA_CIRC_START_BIT);
    DMA_cr |= ((uint32_t)config->data_direction << DMA_DIR_START_BIT);
    DMA_cr |= ((uint32_t)config->periph_flow_crtl << DMA_PFCTRL_START_BIT);
    DMA_cr |= ((uint32_t)config->TC_intrpt_en << DMA_TCIE_BIT);
    DMA_cr |= ((uint32_t)config->HT_intrpt_en << DMA_HTIE_BIT);
    DMA_cr |= ((uint32_t)config->TE_intrpt_en << DMA_TEIE_BIT);
    DMA_cr |= ((uint32_t)config->DME_intrpt_en << DMA_DMEIE_BIT);
    driver->streams[config->stream].CR = DMA_cr;


    driver->streams[config->stream].NDTR = 1UL<<(uint32_t)config->no_of_items;

    driver->streams[config->stream].PAR = config->peripheral_addr;

    /**
        here I am making sure that we don't have a null pointer
        dereference
    */
    BARE_ASSERT(config->mem0_addr != 0);

    if(config->double_buffer_mode == DMA_DB_DIS){
        driver->streams[config->stream].M0AR = config->mem0_addr;
    }else{
        driver->streams[config->stream].M0AR = config->mem0_addr;
        driver->streams[config->stream].M1AR = config->mem1_addr;
        BARE_ASSERT(config->mem1_addr != 0);
    }

    uint32_t dma_sxfcr = driver->streams[config->stream].FCR;
    dma_sxfcr &= DMA_FCR_RESERVED_MSK;
    if(config->mode == DMA_MODE_DIRECT){
        // do nothing cause direct mode is 
        dma_sxfcr &= ~(1UL<<DMA_DIRECT_MODE_BIT);
    }
    else{
        dma_sxfcr |= (1UL<<DMA_DIRECT_MODE_BIT);
        dma_sxfcr |= (config->FIFO_err_intrpt_en<<DMA_FEIE_BIT);
        dma_sxfcr |= (config->fifo_threshold<<DMA_FTH_BIT);
    }

    driver->streams[config->stream].FCR = dma_sxfcr;
    return driver;
}


void DMA_init(DMA_driver_t* driver, DMA_stream_id_t stream){
    DMA_en_stream(driver, stream);
}

DMA_handle_t* DMA_handle_init(DMA_hndl_config_t* config){
    DMA_handle_t* handle_ptr = NULL;
    BARE_ASSERT(config != NULL);

    switch(config->stream){
        case DMA_STREAM_0:
            handle_ptr = &dma0_handle;
            break;
        case DMA_STREAM_1:
            handle_ptr = &dma1_handle;
            break;
        case DMA_STREAM_2:
            handle_ptr = &dma2_handle;
            break;
        case DMA_STREAM_3:
            handle_ptr = &dma3_handle;
            break;
        case DMA_STREAM_4:
            handle_ptr = &dma4_handle;
            break;
        case DMA_STREAM_5:
            handle_ptr = &dma5_handle;
            break;
        case DMA_STREAM_6:
            handle_ptr = &dma6_handle;
            break;
        case DMA_STREAM_7:
            handle_ptr = &dma7_handle;
            break;
        case DMA_BAD_STREAM:
        default:
            __BKPT(0);
    }

    BARE_ASSERT(handle_ptr != NULL);
    BARE_ASSERT(handle_ptr->transfer_state != DMA_STATE_BUSY);
    BARE_ASSERT(config->driver != NULL);
    // BARE_ASSERT(config->HC_callback != NULL);
    BARE_ASSERT(config->TC_callback != NULL);
    // BARE_ASSERT(config->error_callback != NULL);
    BARE_ASSERT(config->user_data != NULL);

    handle_ptr->driver = config->driver;
    handle_ptr->stream = config->stream;
    handle_ptr->HC_callback = config->HC_callback;
    handle_ptr->TC_callback = config->TC_callback;
    handle_ptr->error_callback = config->error_callback;
    handle_ptr->user_data = config->user_data;
    handle_ptr->error_state = DMA_ERROR_NONE;
    handle_ptr->transfer_state = DMA_STATE_READY;

    return handle_ptr;
}

__STATIC_INLINE void dma_isr_handler(DMA_handle_t* handle){
    if(DMA_get_te(handle->driver, handle->stream) == 1){
        DMA_clear_te(handle->driver, handle->stream);
        handle->error_state = DMA_ERROR_TRANSFER;
        handle->transfer_state = DMA_STATE_ERROR;
        if(handle->error_callback != NULL){
            handle->error_callback(handle);
        }
        return;
    }

    if(DMA_get_fe(handle->driver, handle->stream) == 1){
        DMA_clear_fe(handle->driver, handle->stream);
        handle->error_state = DMA_ERROR_FIFO;
        handle->transfer_state = DMA_STATE_ERROR;
        if(handle->error_callback != NULL){
            handle->error_callback(handle);
        }
        return;
    }
    
    if(DMA_get_dme(handle->driver, handle->stream) == 1){
        DMA_clear_dme(handle->driver, handle->stream);
        handle->error_state = DMA_ERROR_DIRECT_MODE;
        handle->transfer_state = DMA_STATE_ERROR;
        if(handle->error_callback != NULL){
            handle->error_callback(handle);
        }
        return;
    }

    if(DMA_get_ht(handle->driver, handle->stream) == 1){
        DMA_clear_ht(handle->driver, handle->stream);
        handle->error_state = DMA_ERROR_NONE;
        handle->transfer_state = DMA_STATE_BUSY;
        if(handle->HC_callback != NULL){
            handle->HC_callback(handle);
        }
    }
    
    if(DMA_get_tc(handle->driver, handle->stream) == 1){
        DMA_clear_tc(handle->driver, handle->stream);
        handle->error_state = DMA_ERROR_NONE;
        if(DMA_get_circ_mode(handle->driver, handle->stream) == 0){
            handle->transfer_state = DMA_STATE_READY;
        }else{
            handle->transfer_state = DMA_STATE_BUSY;
        }

        if(handle->TC_callback != NULL){
            handle->TC_callback(handle);    
        }
    }
}

void DMA1_Stream0_IRQHandler(void){
    dma_isr_handler(&dma0_handle);
}
void DMA1_Stream1_IRQHandler(void){
    dma_isr_handler(&dma1_handle);
}
void DMA1_Stream2_IRQHandler(void){
    dma_isr_handler(&dma2_handle);
}
void DMA1_Stream3_IRQHandler(void){
    dma_isr_handler(&dma3_handle);
}
void DMA1_Stream4_IRQHandler(void){
    dma_isr_handler(&dma4_handle);
}
void DMA1_Stream5_IRQHandler(void){
    dma_isr_handler(&dma5_handle);
}
void DMA1_Stream6_IRQHandler(void){
    dma_isr_handler(&dma6_handle);
}
void DMA1_Stream7_IRQHandler(void){
    dma_isr_handler(&dma7_handle);
}

__STATIC_INLINE void DMA_set_mem0(DMA_driver_t* driver, DMA_stream_id_t stream, uint32_t val){
    driver->streams[stream].M0AR = val;
}

__STATIC_INLINE void DMA_set_mem1(DMA_driver_t* driver, DMA_stream_id_t stream, uint32_t val){
    driver->streams[stream].M1AR = val;
}

__STATIC_INLINE uint32_t DMA_get_CT(DMA_driver_t* driver, DMA_stream_id_t stream){
    return (uint32_t)((driver->streams[stream].CR >> DMA_CT_START_BIT) & 0x1UL);
}

__INLINE void DMA_set_next_buffer(DMA_driver_t* driver, DMA_stream_id_t stream, uint32_t val){
    uint32_t target = DMA_get_CT(driver, stream);
    if(target == DMA_TARGET_MEM_0){
        DMA_set_mem1(driver, stream, val);
    }else if(target ==  DMA_TARGET_MEM_1){
        DMA_set_mem0(driver, stream, val);
    }else{
        __BKPT(0);
        // I told you not to fuck with my enums!
    }
}