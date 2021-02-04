/*
 * Copyright (C) 2017, 2019   Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_bl602
 * @{
 *
 * @file
 * @brief           Memory definitions for the SiFive BL602
 *
 * @author          Ken Rabold
 * @author          Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
INCLUDE bl602_vars.ld

MEMORY
{
  flash (rxai!w) : ORIGIN = _rom_start_addr , LENGTH = _rom_length
  ram   (wxa!ri) : ORIGIN = _ram_start_addr, LENGTH = _ram_length
  itim  (wxa!ri) : ORIGIN = _itim_start_addr, LENGTH = _itim_length
}

INCLUDE bl602_base.ld
