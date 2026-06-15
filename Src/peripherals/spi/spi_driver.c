#include "spi_driver.h"
#include "../../def.h"

struct SPI_driver{
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t CRCPR;
    __IO uint32_t RXCRCR;
    __IO uint32_t TXCRCR;
    __IO uint32_t SPI_I2SCFGR;
    __IO uint32_t SPI_I2SPR;
};

#define SPI1_BASE_ADDR (0x40013000)
#define SPI2_BASE_ADDR (0x40003800)
#define SPI3_BASE_ADDR (0x40003C00)
#define SPI4_BASE_ADDR (0x40013400)

__INLINE SPI_driver_t* SPI_get_instance(SPI_instance_t instance){
    switch(instance){
        case SPI_INSTANCE_1:
            return ((SPI_driver_t*) (SPI1_BASE_ADDR));
        case SPI_INSTANCE_2:
            return ((SPI_driver_t*) (SPI2_BASE_ADDR));
        case SPI_INSTANCE_3:
            return ((SPI_driver_t*) (SPI3_BASE_ADDR));
        case SPI_INSTANCE_4:
            return ((SPI_driver_t*) (SPI4_BASE_ADDR));
        default:
            return NULL;
    }
}

#define SPI_I2SPR_MCKOE_BIT 9UL

void SPI_en_MCK(SPI_driver_t* self){
    bit_band_write((uint32_t)&self->SPI_I2SPR, SPI_I2SPR_MCKOE_BIT, 1);
}

void SPI_dis_MCK(SPI_driver_t *self){
    bit_band_write((uint32_t)&self->SPI_I2SPR, SPI_I2SPR_MCKOE_BIT, 0);
}

#define SPI_I2SPR_ODD_BIT 8UL

void SPI_set_ODD_bit(SPI_driver_t* self){
    bit_band_write((uint32_t)&self->SPI_I2SPR, SPI_I2SPR_ODD_BIT, 1);
}

void SPI_reset_ODD_bit(SPI_driver_t *self){
    bit_band_write((uint32_t)&self->SPI_I2SPR, SPI_I2SPR_ODD_BIT, 0);
}

#define SPI_I2SDIV_FIELD_LEN 8UL
#define SPI_I2SDIV_START_BIT 0UL

__bool SPI_set_I2SDIV(SPI_driver_t* self, uint8_t val){
    if(val == 0 || val == 1){
        return FALSE;
    }

    uint32_t reg_val = self->SPI_I2SPR;
    reg_val &= ~(msk_of_ones(SPI_I2SDIV_FIELD_LEN)<<SPI_I2SDIV_START_BIT);
    reg_val |= ((uint32_t)val << SPI_I2SDIV_START_BIT);
    self->SPI_I2SPR = reg_val;

    return TRUE;
}

#define SPI_I2SMOD_BIT 11UL

void SPI_set_mode(SPI_driver_t* self, SPI_mode_t mode){
    uint32_t addr = (uint32_t)&self->SPI_I2SCFGR;
    if(mode == SPI_SPI_MODE){
        bit_band_write(addr, SPI_I2SMOD_BIT, 0);
    }else if(mode == SPI_I2S_MODE){
        bit_band_write(addr, SPI_I2SMOD_BIT, 1);
    }
}

#define SPI_I2SE_BIT 10UL

void SPI_en_I2S(SPI_driver_t* self){
    bit_band_write((uint32_t)&self->SPI_I2SCFGR, SPI_I2SE_BIT, 1);
}

void SPI_dis_I2S(SPI_driver_t *self){
    bit_band_write((uint32_t)&self->SPI_I2SCFGR, SPI_I2SE_BIT, 0);
}

#define SPI_I2SCFG_START_BIT 8UL
#define SPI_I2SCFG_FIELD_LEN 2UL

void SPI_set_I2S_conf(SPI_driver_t* self, I2S_conf_t conf){
    uint32_t reg_val = self->SPI_I2SCFGR;
    reg_val &= ~(msk_of_ones(SPI_I2SCFG_FIELD_LEN)<<SPI_I2SCFG_START_BIT);
    reg_val |= ((uint32_t)conf<<SPI_I2SCFG_START_BIT);
    self->SPI_I2SCFGR = reg_val;
}

#define SPI_I2SSTD_START_BIT 4UL
#define SPI_I2SSTD_FIELD_LEN 2UL

void SPI_set_I2S_std(SPI_driver_t* self, I2S_std_t std){
    uint32_t reg_val = self->SPI_I2SCFGR;
    reg_val &= ~(msk_of_ones(SPI_I2SSTD_FIELD_LEN)<<SPI_I2SSTD_START_BIT);
    reg_val |= ((uint32_t)std<<SPI_I2SSTD_START_BIT);
    self->SPI_I2SCFGR = reg_val;
}

#define SPI_DATLEN_START_BIT 1UL
#define SPI_DATLEN_FIELD_LEN 2UL

void SPI_set_I2S_data_len(SPI_driver_t* self, I2S_data_len_t len){
    uint32_t reg_val = self->SPI_I2SCFGR;
    reg_val &= ~(msk_of_ones(SPI_DATLEN_FIELD_LEN)<<SPI_DATLEN_START_BIT);
    reg_val |= ((uint32_t)len<<SPI_DATLEN_START_BIT);
    self->SPI_I2SCFGR = reg_val;
}

#define SPI_CHANLEN_BIT 0UL

void SPI_set_I2S_chan_len(SPI_driver_t* self, I2S_chan_len_t len){
    uint32_t addr = (uint32_t)&self->SPI_I2SCFGR;
    switch(len){
        case I2S_CHAN_LEN_16:
            bit_band_write(addr, SPI_CHANLEN_BIT, 0);
            break;
        case I2S_CHAN_LEN_32:
            bit_band_write(addr, SPI_CHANLEN_BIT, 1);
            break;
        default:
            return;
    }
}

#define I2S_CKPOL_BIT 3UL

void SPI_set_I2S_ckpol(SPI_driver_t* self, I2S_ckpol_t val){
    uint32_t addr = (uint32_t)&self->SPI_I2SCFGR;
    switch(val){
        case I2S_CKPOL_LOW:
            bit_band_write(addr, I2S_CKPOL_BIT, 0);
            break;
        case I2S_CKPOL_HIGH:
            bit_band_write(addr, I2S_CKPOL_BIT, 1);
            break;
        default:
            return;
    }
}