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
 * The CST816S is a touch sensor from Hynitron with integrated gesture
 * detection. It is able to measure both the position of a single finger and a
 * number of basic gestures. The PineTime board has one of these for the touch
 * screen.
 *
 * Documentation about the specifics is very limited and most of this driver is
 * based on experimenting with the chip and from community effort on the
 * PineTime.
 *
 * Two things about the driver are noteworthy:
 *  1. It only responds to I2C commands after an event, such as a touch
 *     detection. Do not expect it to respond on init. Instead after a touch
 *     event, it will assert the IRQ and respond to I2C reads for a short time.
 *  2. While it should be able to detect multiple finger events, this version of
 *     the chip always returns only a single finger event and a gesture.
 *
 * Reading the display data multiple times during a single event will return the
 * last sampled finger position.
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

/**
 * @brief cst816s_t forward declaration
 */
typedef struct _cst816s cst816s_t;

/**
 * @brief callback definition
 */
typedef void (*cst816s_irq_cb_t)(cst816s_t *dev, void *arg);

/**
 * @brief cst816s touch event touch state
 */
typedef enum {
    CST816S_TOUCH_DOWN      = 0,    /**< Touch press */
    CST816S_TOUCH_UP        = 1,    /**< Touch release */
    CST816S_TOUCH_CONTACT   = 2,    /**< Touch contact */
} cst816s_touch_t;

/**
 * @brief CST816S Gesture types
 */
typedef enum {
    CST816S_GESTURE_NONE            = 0x00, /**< no gesture detecte        */
    CST816S_GESTURE_SLIDE_DOWN      = 0x01, /**< downward slide detected   */
    CST816S_GESTURE_SLIDE_UP        = 0x02, /**< upward slide detected     */
    CST816S_GESTURE_SLIDE_LEFT      = 0x03, /**< left slide detected       */
    CST816S_GESTURE_SLIDE_RIGHT     = 0x04, /**< right slide detected      */
    CST816S_GESTURE_SINGLE_CLICK    = 0x05, /**< single click detected     */
    CST816S_GESTURE_DOUBLE_CLICK    = 0x0b, /**< double click detected     */
    CST816S_GESTURE_LONG_PRESS      = 0x0c, /**< long press detected       */
} cst816s_gesture_t;

/**
 * @brief cst816s touch event data
 */
typedef struct {
    cst816s_gesture_t gesture;  /**< Detected gesture */
    cst816s_touch_t action;     /**< Press or release event */
    uint16_t x;                 /**< X coordinate */
    uint16_t y;                 /**< Y coordinate */
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
    CST816S_OK      =  0,           /**< everything was fine */
    CST816S_ERR_IRQ = -1,           /**< IRQ initialization error */
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
