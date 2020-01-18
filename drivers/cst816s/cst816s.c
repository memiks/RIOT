/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cst816s
 * @{
 *
 * @file
 * @brief       Device driver implementation for cst816s touch screen
 *
 * @author      koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "log.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "xtimer.h"

#include "cst816s.h"
#include "cst816s_internal.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static void _gpio_irq(void *arg)
{
    cst816s_t *dev = arg;
    dev->cb(dev, dev->cb_arg);
}

static void _cst816s_reset(cst816s_t *dev)
{
    /* Reset, sleep durations based on
     * https://github.com/lupyuen/hynitron_i2c_cst0xxse/blob/master/cst0xx_core.c#L1078-L1085 */
    gpio_clear(dev->params->reset);
    xtimer_usleep(CST816S_RESET_DURATION_LOW);
    gpio_set(dev->params->reset);
    xtimer_usleep(CST816S_RESET_DURATION_HIGH);
}

#if 0
static uint16_t _cst816s_chip_id(cst816s_t *dev)
{
    uint16_t id = 0;
    i2c_acquire(dev->params->i2c_dev);
    int res = i2c_read_reg(dev->params->i2c_dev, dev->params->i2c_addr,
                 CST816S_REG_FW_VER, &id, 0);
    if (res < 0) {
        DEBUG("[cst816s]: Error reading chip id device %d\n", res);
    }
    i2c_release(dev->params->i2c_dev);
    DEBUG("[cst816s]: chip id is %d.\n", id);
    return id;
}
#endif

int cst816s_suspend(cst816s_t *dev)
{
    gpio_irq_disable(dev->params->irq);
    i2c_acquire(dev->params->i2c_dev);
    int res = i2c_write_reg(dev->params->i2c_dev, dev->params->i2c_addr,
                            0xA5, 0x05, 0);
    i2c_release(dev->params->i2c_dev);
    if (res < 0) {
        DEBUG("[cst816s]: Error suspending the device %d\n", res);
    }
    return res;
}

void cst816s_resume(cst816s_t *dev)
{
    _cst816s_reset(dev);
    gpio_irq_enable(dev->params->irq);
}

int cst816s_read(cst816s_t *dev, cst816s_touch_data_t *data, size_t num)
{
    uint8_t buf[64];
    size_t num_points_found = 0;

    i2c_acquire(dev->params->i2c_dev);

    int res = i2c_read_regs(dev->params->i2c_dev, dev->params->i2c_addr,
                            0, buf, sizeof(buf), 0);
    i2c_release(dev->params->i2c_dev);
    if (res < 0) {
        return res;
    }
    uint8_t points = buf[2] & 0x0f;
    DEBUG("[cst816s] Number of points: %u\n", points);

    size_t max_points_copy = points < num ? points : num;
    for (size_t i = 0; i < max_points_copy; i++) {
        uint8_t *point_buf = &buf[3 + 6 * i];
        uint8_t point_id = point_buf[2] >> 4;
        if (point_id > 0x0f) {
            break;
        }
        num_points_found++;

        data[i].finger = point_id;
        data[i].x = (point_buf[0] & 0x0f) << 8 | point_buf[1];
        data[i].y = (point_buf[2] & 0x0f) << 8 | point_buf[3];

        data[i].action = point_buf[0] >> 6;
    }

    return num_points_found;
}

int cst816s_init(cst816s_t *dev, const cst816s_params_t *params,
                 cst816s_irq_cb_t cb, void *arg)
{
    assert(dev && params);
    dev->params = params;
    dev->cb = cb;
    dev->cb_arg = arg;

    gpio_init(dev->params->reset, GPIO_OUT);
    _cst816s_reset(dev);

    if (cb) {
        int res = gpio_init_int(dev->params->irq, GPIO_IN, dev->params->irq_flank,
                                _gpio_irq, dev);
        if (res < 0) {
            return CST816S_ERR_IRQ;
        }
    }
    return CST816S_OK;
    /* The device will not respond until the first touch event */
}
