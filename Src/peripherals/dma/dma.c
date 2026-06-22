#include "dma.h"

#include <stdint.h>
#include "../../def.h"
#include "Src/arm/arm.h"

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
}


typedef enum intrpt_reg{
    FIFO_ERROR = 0,
    DIRECT_MODE_ERROR = 2,
    TRANSFER_ERROR = 3,
    HALF_TRANSFER = 4,
    TRANSFER_COMPLETE = 5
}intrpt_reg_t;

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
}

__INLINE void DMA_clear_tc(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_COMPLETE
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);
}

__INLINE void DMA_clear_ht(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        HALF_TRANSFER 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__INLINE void DMA_clear_te(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        TRANSFER_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__INLINE void DMA_clear_dme(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        DIRECT_MODE_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__INLINE void DMA_clear_fe(DMA_driver_t* self, DMA_stream_id_t stream_id){
    uint32_t ifcr_idx = stream_id/4;
    uint32_t clear_bit = DMA_get_intrpt_bit(
        stream_id, 
        FIFO_ERROR 
    );
    self->IFCR[ifcr_idx] = (1UL<<clear_bit);   
}

__INLINE uint32_t DMA_get_tc(DMA_driver_t* self, DMA_stream_id_t stream_id){
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

__INLINE uint32_t DMA_get_ht(DMA_driver_t* self, DMA_stream_id_t stream_id){
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
__INLINE uint32_t DMA_get_te(DMA_driver_t* self, DMA_stream_id_t stream_id){
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
__INLINE uint32_t DMA_get_dme(DMA_driver_t* self, DMA_stream_id_t stream_id){
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
__INLINE uint32_t DMA_get_fe(DMA_driver_t* self, DMA_stream_id_t stream_id){
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

__INLINE void DMA_en_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
    bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_TCIE_BIT, 
        1
    );
}

#define DMA_HTIE_BIT 3UL

__INLINE void DMA_en_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_HTIE_BIT, 
        1
    );   
}

#define DMA_TEIE_BIT 2UL

__INLINE void DMA_en_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_TEIE_BIT, 
        1
    );   
}

#define DMA_DMEIE_BIT 1UL

__INLINE void DMA_en_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        1
    );   
}


__INLINE void DMA_dis_TC_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__INLINE void DMA_dis_HT_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__INLINE void DMA_dis_TE_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}
__INLINE void DMA_dis_DME_intpt(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_DMEIE_BIT, 
        0
    );   
}

#define DMA_EN_BIT 0UL

__INLINE void DMA_en_stream(DMA_driver_t* self, DMA_stream_id_t stream_id){
    bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_EN_BIT, 
        1
    );      
}

__INLINE void DMA_dis_stream(DMA_driver_t* self, DMA_stream_id_t stream_id){
     bit_band_write(
        (uint32_t)&self->streams[stream_id].CR,
        DMA_EN_BIT, 
        0
    );         
}
