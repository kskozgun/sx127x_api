#include "lora_api.h"

enum sx127x_err sx127x_lora_init(sx127x_dev *dev)
{
    int8_t spi_ret;
    enum sx127x_err ret;
    uint8_t data;
    uint8_t agc_on = 0X04; // Automatic Gain Control

    spi_ret = dev->spi_read(SX127X_REG_VERSION, &data, 1);
    if (ret) {
        return sx127x_SPI_Error;
    }
    
    if(data != SX127X_VERSION) {
        return sx127x_Unsupported_Module;
    }

    /* Enter sleep mode */
    ret = sx127x_lora_set_mode(dev, SX127X_SLEEP);
    if (ret) {
        return ret;
    }

    /* Set Frequency */
    ret = sx127x_lora_set_freq(dev);
    if (ret) {
        return ret;
    }

    /* Tx and Rx use the whole data buffer seperately */
    ret = sx127x_lora_set_TXRXbaseaddr(dev);
    if (ret) {
        return ret;
    }
    
    /* RX Automatic Gain Control */
    spi_ret = dev->spi_write(SX127X_REG_CONFIG_3, &agc_on, 1);
    if (ret) {
        return sx127x_SPI_Error;
    }
    
    /* Set Transmission Power in dBm */
    ret = sx127x_lora_set_TxPow(dev);
    if (ret) {
        return ret;
    }
    /* Set standby mode */
    ret = sx127x_lora_set_mode(dev, SX127X_STDBY);
    if (ret) {
        return ret;
    }    
}

static enum sx127x_err setOCP(sx127x_dev *dev, uint8_t mA)
{
    enum sx127x_err ret;
    uint8_t ocpTrim = 27;
    uint8_t data;

    if (mA <= 120) {
        ocpTrim = (mA - 45) / 5;
    } else if (mA <=240) {
        ocpTrim = (mA + 30) / 10;
    }
  
    data = 0x20 | (0x1F & ocpTrim);

    ret = dev->spi_write(0x0B, &data, 1);
    if (ret) {
        return sx127x_SPI_Error;
    }
}

enum sx127x_err sx127x_lora_set_freq(sx127x_dev *dev)
{
    enum sx127x_err ret;
    uint64_t frf_temp = 0;
    uint8_t frf[3] = {0};

    /* (freq) * (2^19) / (Fxosc) */
    frf_temp = (dev->freq << 19) / SX127X_FXOSC;
    frf[0] = frf_temp >> 16;
    frf[1] = frf_temp >> 8;
    frf[2] = frf_temp;

    ret = dev->spi_write(SX127X_REG_RFMSB, frf, 3);
    if (ret) {
        return sx127x_SPI_Error;
    }
    return sx127x_OK;
}

enum sx127x_err sx127x_lora_set_mode(sx127x_dev *dev, uint8_t mode)
{
    enum sx127x_err ret;
    uint8_t dev_mode;

    dev_mode = (mode | 0x80);

    ret = dev->spi_write(SX127X_REG_OPMODE, &dev_mode, 1);
    if(ret) {
        return sx127x_SPI_Error;
    }
    return sx127x_OK;
}

enum sx127x_err sx127x_lora_set_TXRXbaseaddr(sx127x_dev *dev)
{
    enum sx127x_err ret;
    uint8_t data = 0;

    /* FIFO address pointer */
    ret = dev->spi_write(SX127X_REG_FIFO_TX_ADDR, &data, 2);
    if (ret) {
        return sx127x_SPI_Error;
    }
    return sx127x_OK;
}

enum sx127x_err sx127x_lora_set_sf(sx127x_dev *dev)
{
    enum sx127x_err ret;
    uint8_t sf;
    sf = (dev->sf) << 4;
    ret = dev->spi_write(SX127X_REG_CONFIG_2, &sf, 1);
    if (ret < 0){
        return sx127x_SPI_Error;
    }
    return sx127x_OK;
}

//Will be upgraded in the future ( soon or later :) ) 
enum sx127x_err sx127x_lora_set_TxPow(sx127x_dev *dev)
{
    int8_t spi_ret;
    enum sx127x_err ret;
    uint8_t txpow;
    uint8_t max_txpow = 0x87;

    /* Support only PA_BOOST for now */
    /* from +2 to 20 dBm */
    if(dev->txpow > 20) {
        dev ->txpow = 20;
    }
    else if (dev->txpow == 20) {
        /* Only for 20 dBm */
        spi_ret = dev->spi_write(SX127X_REG_PaDac, &max_txpow, 1);
        if (ret) {
            return sx127x_SPI_Error;
        }
    }
    else if (dev->txpow <= 17) {
        txpow = (dev->txpow - 2) | (0x80);
        spi_ret = dev->spi_write(SX127X_REG_PACONF, &txpow, 1);
        if (ret) {
            return sx127x_SPI_Error;
        }
    }
    return sx127x_OK;
}
//set bw,cr,header_mode,sf,crc_enabled_disabled 
enum sx127x_err sx127x_lora_tx_data(sx127x_dev *dev, uint8_t *tx_data, uint8_t len)
{
    enum sx127x_err ret;
    uint8_t txdone_mask;
    uint8_t data = 0;
    uint8_t payload_len = len;
    uint8_t clear_irq = 0x08;
    int i = 0;
    
    /* Default sf = 7, cr = 4/5, bw = 125 KHz, explicit header_mode, crc disabled, tx single */
    /* These parameters are in config_1 and config_2 register */
    
    ret = sx127x_lora_set_mode(dev, SX127X_STDBY);
    if (ret) {
        return ret;
    }
    
    /* Set FIFO Addr 0 */
    ret = dev->spi_write(SX127X_REG_FIFO_ADDR_PTR, &data, 1);
    if (ret < 0) {
        return ret;
    }

    /* Set Payload length at 0 */
    ret = dev->spi_write(SX127X_PAYLOAD_LEN, &data, 1);
    if (ret < 0) {
        return ret;
    }    

    /* Write data in FIFO */
    for(i = 0; i < len; i++) {
        ret = dev->spi_write(SX127X_REG_FIFO, &tx_data[i], 1);
        if (ret < 0) {
            return ret;
        }
    }

    /* Update payload length */
    ret = dev->spi_write(SX127X_PAYLOAD_LEN, &payload_len , 1);
    if (ret < 0) {
        return ret;
    }

    /* DIO0 TxDone - 0x01  
    ret = dev->spi_write(SX127X_REG_DIOMAP, &dio0_txdone, 1);
    if (ret) {
        return sx127x_SPI_Error;
    } */

    /* Tx Mode */
    ret = sx127x_lora_set_mode(dev, SX127X_TX);
    if (ret) {
        return ret;
    }

    /* polling for tx done */
    do {
        ret = dev->spi_read(SX127X_REG_IRQFLAG, &txdone_mask, 1);
        if (ret) {
            return sx127x_SPI_Error;
        }
        //dev->delay_ms(15);
    } while(!(txdone_mask & 0x08));

    /* Clear interrupt */
    ret = dev->spi_write(SX127X_REG_IRQFLAG, &clear_irq, 1);
    if (ret) {
        return sx127x_SPI_Error;
    }    

    ret = sx127x_lora_set_mode(dev, SX127X_STDBY);
    if (ret) {
        return sx127x_SPI_Error;
    }
    return sx127x_OK;
}

enum sx127x_err sx127x_lora_rx_data(sx127x_dev *dev)
{
    enum sx127x_err ret;
    uint8_t rxdone_dio = 0x00;
    
    /* DIO0 is mapped for RxDone */
    ret = dev->spi_write(SX127X_REG_DIOMAP, &rxdone_dio, 1);
    if (ret) {
        return sx127x_SPI_Error;
    }

    /* Rx Continuous Mode */
    ret = sx127x_lora_set_mode(dev, SX127X_RXC);
    if (ret) {
        return ret;
    }
    return sx127x_OK;
}