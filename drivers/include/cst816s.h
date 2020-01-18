/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_cst816s Cst816S touch screen driver
 *
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the Hynitron CST816S touch screen
 *
 * @{
 * @file
 * @brief       Device driver interface for the CST816S touch screen
 *
 * @author      koen Zandberg <koen@bergzand.net>
 */

#ifndef CST816S_H
#define CST816S_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _cst816s cst816s_t;

typedef void (*cst816s_irq_cb_t)(cst816s_t *dev, void *arg);

/**
 * @brief cst816s touch event touch state
 */
typedef enum {
    CST816S_TOUCH_DOWN = 0,
    CST816S_TOUCH_UP = 1,
    CST816S_TOUCH_CONTACT = 2,
} cst816s_touch_t;

/**
 * @brief cst816s touch event data
 */
typedef struct {
    uint16_t x;         /**< X coordinate */
    uint16_t y;         /**< Y coordinate */
    uint8_t  action;    /**< One of @ref cst816s_touch_t */
    uint8_t  finger;    /**< Finger index */
    uint8_t  pressure;  /**< Pressure of touch */
    uint8_t  area;      /**< touch area */
} cst816s_touch_data_t;

typedef struct {
    /* I2C details */
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
    gpio_t irq;                         /**< IRQ pin */
    gpio_flank_t irq_flank;             /**< IRQ flank */
    gpio_t reset;                       /**< Device reset GPIO */
} cst816s_params_t;

/**
 * @brief cst816s device descriptor
 */
struct _cst816s {
    const cst816s_params_t *params;     /**< Device parameters */
    cst816s_irq_cb_t cb;                /**< Configured IRQ event callback */
    void *cb_arg;                       /**< Extra argument for the callback */
};

/**
 * @brief   Status and error return codes
 */
enum {
    CST816S_OK           =  0,     /**< everything was fine */
    CST816S_ERR_IRQ      = -1,     /**< IRQ initialization error */
};

/**
 * @brief   Initialize the given cst816s device
 *
 * @param[out] dev      device descriptor of the given cst816s device
 * @param[in]  params   static configuration parameters
 * @param[in]  cb       callback for the cst816s event interrupt, may be NULL
 * @param[in]  arg      extra argument passed to the event interrupt.
 *
 * @returns             CST816S_OK on success
 * @returns             CST816S_ERR_IRQ on IRQ initialization error
 */
int cst816s_init(cst816s_t *dev, const cst816s_params_t *params,
                 cst816s_irq_cb_t cb, void *arg);

/**
 * @brief   Read touch data from the cst816s device
 *
 * @param[in]   dev     device descriptor
 * @param[out]  data    Touch data array
 * @param[in]   num     Number of entries in @p data
 *
 * @returns             the number of touch entries available in @p data
 * @returns             negative on I2C access error
 */
int cst816s_read(cst816s_t *dev, cst816s_touch_data_t *data, size_t num);

/**
 * @brief   Suspend the given cst816s device
 *
 * @param[in]   dev     device descriptor
 *
 * @returns             CST816S_OK on success
 * @returns             negative on I2C access error
 */
int cst816s_suspend(cst816s_t *dev);

/**
 * @brief   Resume the given cst816s device
 *
 * @param[in]   dev     device descriptor
 */
void cst816s_resume(cst816s_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* CST816_H */
/** @} */
