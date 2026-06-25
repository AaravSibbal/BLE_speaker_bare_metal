#include "i2s.h"
#include "../../def.h"
#include "../../arm/arm.h"
#include "../gpio/gpio.h"
#include "../rcc/rcc.h"
#include "Src/arm/arm.h"
#include "Src/peripherals/dma/dma.h"
#include "spi_driver.h"
#include "stdint.h"

static const GPIO_port_t I2S3_MCK_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_MCK_GPIO_PIN = GPIO_PIN_7;
// static const GPIO_port_t I2S3_SCK_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_SCK_GPIO_PIN = GPIO_PIN_10;
// static const GPIO_port_t I2S3_SD_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_SD_GPIO_PIN = GPIO_PIN_12;
static const GPIO_port_t I2S3_WS_GPIO_PORT = GPIO_PORT_A;
static const GPIO_Pin_t I2S3_WS_GPIO_PIN = GPIO_PIN_4;

static const GPIO_MODER_t I2S_gpio_mode = GPIO_MODE_ALT;
static const GPIO_AFx_t I2S3_gpio_af = AF6;
static const GPIO_OTYPER_t I2S_gpio_otype = GPIO_TYPE_PUSH_PULL;
static const GPIO_PUPDR_t I2S_gpio_pupd = NO_PUPD;
static const GPIO_OSPEEDR_t I2S_gpio_speed = OSPEED_HIGH;

static volatile uint16_t DMA_mem0_buffer[256] = { 0 }; 

__STATIC_INLINE void i2s_conf_gpio(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_AFx_t af_val){
    GPIO_set_alt_func(gpio, pin, af_val);
    GPIO_set_otyper(gpio, pin, I2S_gpio_otype);
    GPIO_set_ospeedr(gpio, pin, I2S_gpio_speed);
    GPIO_set_pupdr(gpio, pin, I2S_gpio_pupd);
    GPIO_set_moder(gpio, pin, I2S_gpio_mode);
}

__STATIC_INLINE void i2s_init_gpio(I2S_instance_t instance, RCC_t* rcc){
    switch (instance) {
        case I2S_INSTANCE_2:
            // not implemented yet
            __BKPT(0);
            break;
        case I2S_INSTANCE_3:{
            GPIO_t* gpio_c = GPIO_init(I2S3_MCK_GPIO_PORT, rcc);
            GPIO_t* gpio_a = GPIO_init(I2S3_WS_GPIO_PORT, rcc);
            i2s_conf_gpio(gpio_c, I2S3_MCK_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_c, I2S3_SCK_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_c, I2S3_SD_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_a, I2S3_WS_GPIO_PIN, I2S3_gpio_af);
            break;
        }
        default:
            return;
    }
}

__STATIC_INLINE void i2s_init_clocks(I2S_instance_t instance, RCC_t* rcc){
    switch (instance) {
        case I2S_INSTANCE_2:
            RCC_en_SPI2(rcc);
            break;
        case I2S_INSTANCE_3:
            RCC_en_SPI3(rcc);
            break;
        default:
            return;
    }
}

/**
for a 48000 targer sample rate 
*/

#define PLLI2SN_48K 258UL
#define PLLI2SR_48K 3UL
#define I2SDIV_48K ((uint8_t)3)

__STATIC_INLINE void i2s_en_PLLI2S(RCC_t* rcc){
    RCC_set_PLLI2SN(rcc, PLLI2SN_48K);
    RCC_set_PLLI2SR(rcc, PLLI2SR_48K);
    RCC_en_PLLI2S(rcc);
    while(RCC_get_PLLI2S_ready(rcc) == 0){

    }
}

__STATIC_INLINE DMA_channel_t I2S_get_DMA_stream(I2S_instance_t instance, I2S_mode_t mode){
    if(instance == I2S_INSTANCE_3 && mode == I2S_MODE_DMA_RX){
        return DMA_STREAM_0;
    }else if (instance == I2S_INSTANCE_3 && mode == I2S_MODE_DMA_TX) {
        return DMA_STREAM_5;
    }else if(instance == I2S_INSTANCE_2 && mode == I2S_MODE_DMA_RX){
        return DMA_STREAM_3;
    }else if(instance == I2S_INSTANCE_2 && mode == I2S_MODE_DMA_TX){
        return DMA_STREAM_4;
    }
    return DMA_BAD_STREAM;
}


void i2s_init(I2S_instance_t instance, RCC_t* rcc, I2S_mode_t mode){
    i2s_en_PLLI2S(rcc);
    
    i2s_init_gpio(instance, rcc);
    i2s_init_clocks(instance, rcc);

    SPI_instance_t spi_instance;
    switch(instance){
        case I2S_INSTANCE_2:
            spi_instance = SPI_INSTANCE_2;
            break;
        case I2S_INSTANCE_3:
            spi_instance = SPI_INSTANCE_3;
            break;
        default:
            __BKPT(0);
            return;
    }

    SPI_driver_t* spi_driver = SPI_get_instance(spi_instance);
    SPI_dis_I2S(spi_driver);
    
    SPI_set_I2SDIV(spi_driver, I2SDIV_48K);
    SPI_set_ODD_bit(spi_driver);
    SPI_en_MCK(spi_driver);
    SPI_set_mode(spi_driver, SPI_I2S_MODE);
    SPI_set_I2S_std(spi_driver, I2S_PHILLIPS);
    SPI_set_I2S_conf(spi_driver, I2S_MASTER_TRANSMIT);
    SPI_set_I2S_ckpol(spi_driver, I2S_CKPOL_LOW);
    SPI_set_I2S_data_len(spi_driver, I2S_DATA_LEN_16);
    SPI_set_I2S_chan_len(spi_driver, I2S_CHAN_LEN_16);
    SPI_set_err_intrpt(spi_driver, SPI_EN);

    if(mode == I2S_MODE_DMA_TX){
        SPI_set_DMATX(spi_driver, SPI_EN);
        DMA_config_t dma_config = {
            .stream = I2S_get_DMA_stream(instance, mode),
            .channel = DMA_CHANNEL_0,
            .memory_burst = DMA_INCR_SINGLE,
            .peripheral_burst = DMA_INCR_SINGLE,
            .curr_target = DMA_TARGET_MEM_0,
            .double_buffer_mode = DMA_DB_DIS,
            .priority_level = DMA_PRIORITY_HIGH,
            .peripheral_incrmnt_offset = DMA_PINCOS_LINKED_TO_PSIZE,
            .memory_data_size = DMA_MEM_16_BIT,
            .peripheral_data_size = DMA_MEM_16_BIT,
            .mem_incr_mode = DMA_MINC_INCREMENT,
            .periph_incr_mode = DMA_PINC_FIXED,
            .circ_mode = DMA_CIRC_EN,
            .data_direction = DMA_DIR_MEM_TO_PERIPH,
            .periph_flow_crtl = DMA_CTRL_DMA,
            .TC_intrpt_en = TRUE,
            .HT_intrpt_en = TRUE,
            .TE_intrpt_en = TRUE,
            .DME_intrpt_en = TRUE,
            .no_of_items = 256,
            .peripheral_addr = SPI_get_DR_addr(spi_driver),
            .mem0_addr = (uint32_t)DMA_mem0_buffer,
            .mem1_addr = 0,
            .mode = DMA_MODE_DIRECT,
            .FIFO_err_intrpt_en = FALSE,
            .fifo_threshold = DMA_FIFO_FULL // doesn't matter
        };
        DMA_driver_t* dma_driver = DMA_init(&dma_config, DMA_INSTANCE_1, rcc);

    }else if(mode == I2S_MODE_DMA_RX){
        SPI_set_DMARX(spi_driver, SPI_EN);
        // not implemented the config yet
        __BKPT(0);
    }

    SPI_en_I2S(spi_driver);
    
}

SPI3_IRQHandler(){
    static const 
}