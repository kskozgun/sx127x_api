#ifndef _LORA_API_H
#define _LORA_API_H

#include <stdio.h>
#include <stdint.h>

#define SX127X_REG_FIFO          (0x00)
#define SX127X_REG_OPMODE        (0x01)
#define SX127X_REG_RFMSB         (0x06)
#define SX127X_REG_RFMID         (0x07)
#define SX127X_REG_RFLSB         (0x08)
#define SX127X_REG_PACONF        (0x09)
#define SX127X_REG_IRQFLAG       (0x12)
#define SX127X_PAYLOAD_LEN       (0x22)
#define SX127X_REG_VERSION       (0x42)
#define SX127X_REG_FIFO_ADDR_PTR (0x0D)
#define SX127X_REG_FIFO_TX_ADDR  (0x0E)
#define SX127X_REG_FIFO_RX_ADDR  (0x0F)
#define SX127X_REG_CONFIG_1      (0x1D)
#define SX127X_REG_CONFIG_2      (0x1E)
#define SX127X_REG_CONFIG_3      (0x26)
#define SX127X_REG_DIOMAP        (0x40)
#define SX127X_REG_VERSION       (0x42)
#define SX127X_REG_PaDac         (0x4D)

/* SX127X Version */
#define SX127X_VERSION 0x12

/* Frequency Settings */
#define SX127X_FXOSC 32e6

/* Device Modes */
#define SX127X_SLEEP 0x00
#define SX127X_STDBY 0x01
#define SX127X_FSTX  0x02
#define SX127X_TX    0x03
#define SX127X_FSRX  0x04
#define SX127X_RXC   0x05
#define SX127X_RXS   0x06


enum sx127x_err {
    sx127x_OK = 0,
    sx127x_Unsupported_Module = -1,
    sx127x_SPI_Error = -2,
};


typedef struct {
    uint64_t freq;
    uint32_t bw;
    uint8_t sf;
    uint8_t cr;
    uint8_t txpow;
    int8_t (*spi_read)(uint8_t addr, uint8_t *data, uint8_t len);
    int8_t (*spi_write)(uint8_t addr, uint8_t *data, uint8_t len);
    void (*delay_ms)(uint16_t ms);
} sx127x_dev;


enum sx127x_err sx127x_lora_init(sx127x_dev *dev);

enum sx127x_err sx127x_lora_set_mode(sx127x_dev *dev, uint8_t mode);

enum sx1278_err sx127x_lora_read_mode(sx127x_dev *dev);

enum sx127x_err sx127x_lora_set_freq(sx127x_dev *dev);

enum sx127x_err sx127x_lora_set_TXRXbaseaddr(sx127x_dev *dev);

enum sx127x_err sx127x_lora_set_sf(sx127x_dev *dev);

enum sx127x_err sx127x_lora_set_TxPow(sx127x_dev *dev);


/**
 * @brief This function transmit data that is in dev->txpow 
 */
enum sx127x_err sx127x_lora_tx_data(sx127x_dev *dev, uint8_t *tx_data, uint8_t len);



#endif // _LORA_API_H_