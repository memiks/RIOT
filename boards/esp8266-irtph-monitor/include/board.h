/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp8266-irtph-monitor
 * @brief       Board specific definitions for
 *              Indoor Corgi esp-irtph-monitor boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    On-board LED configuration and control definitions
 * @{
 */
#define LED0_PIN    GPIO14   /**< GPIO13 is used as LED Pin */
#define LED0_ACTIVE (1)     /**< LED is high active */

#define LED1_PIN    GPIO14   /**< GPIO14 is used as LED Pin */
#define LED1_ACTIVE (1)     /**< LED is high active */

#define LEDIROUT_PIN    GPIO4   /**< GPIO13 is used as LED Pin */
#define LEDIROUT_ACTIVE (1)     /**< LED is high active */

#define LEDIRIN_PIN    GPIO5   /**< GPIO13 is used as LED Pin */
#define LEDIRIN_ACTIVE (1)     /**< LED is high active */

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
