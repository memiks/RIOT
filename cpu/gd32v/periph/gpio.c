/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           GD32V GPIO implementation
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#include "cpu.h"
#include "periph_cpu.h"

/**
 * @brief   Extract information from mode parameter
 */
#define MODE_MASK                       (0x0f)
#define ODR_POS                         (4U)

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_Type *_port(gpio_t pin)
{
    return (GPIO_Type *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 10) & 0x0f) - 2);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline void _port_enable_clock(gpio_t pin)
{
    periph_clk_en(APB2, (RCU_APB2EN_PAEN_Msk << _port_num(pin)));
}

/**
 * @brief   Check if the given mode is some kind of input mdoe
 * @param[in]   mode    Mode to check
 * @retval  1           @p mode is GPIO_IN, GPIO_IN_PD, or GPIO_IN_PU
 * @retval  0           @p mode is not an input mode
 */
static inline int gpio_mode_is_input(gpio_mode_t mode)
{
    return !(mode & 3);
}

static inline void _set_mode_or_af(GPIO_Type *port, unsigned pin_num,
                                   unsigned mode_or_af)
{
    volatile uint32_t *crl = (&port->CTL0 + (pin_num >> 3));
    uint32_t tmp = *crl;

    tmp &= ~(0xf << ((pin_num & 0x7) * 4));
    tmp |= ((mode_or_af & MODE_MASK) << ((pin_num & 0x7) * 4));
    *crl = tmp;
}

static inline bool _pin_is_output(GPIO_Type *port, unsigned pin_num)
{
    uint32_t reg = *(uint32_t *)(&port->CTL0 + (pin_num >> 3));

    return reg & (0x3 << ((pin_num & 0x7) << 2));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    /* open-drain output with pull-up is not supported */
    if (mode == GPIO_OD_PU) {
        return -1;
    }

    /* enable the clock for the selected port */
    _port_enable_clock(pin);

    /* set pin mode */
    _set_mode_or_af(port, pin_num, mode);

    /* For input modes, ODR controls pull up settings */
    if (gpio_mode_is_input(mode)) {
        if (mode == GPIO_IN_PU) {
            cpu_reg_enable_bits(&port->OCTL, 1 << pin_num);
        }
        else {
            cpu_reg_disable_bits(&port->OCTL, 1 << pin_num);
        }
    }

    return 0; /* all OK */
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    /* enable the clock for the selected port */
    _port_enable_clock(pin);
    /* configure the pin */
    _set_mode_or_af(port, pin_num, af);
}

void gpio_init_analog(gpio_t pin)
{
    /* enable the GPIO port RCC */
    _port_enable_clock(pin);

    /* map the pin as analog input */
    int pin_num = _pin_num(pin);

    *(uint32_t *)(&_port(pin)->CTL0 +
                  (pin_num >=
                   8)) &= ~(0xfl << (4 * (pin_num - ((pin_num >= 8) * 8))));
}

int gpio_read(gpio_t pin)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    if (_pin_is_output(port, pin)) {
        /* pin is output */
        return (port->OCTL & (1 << pin_num));
    }
    else {
        /* or input */
        return (port->ISTAT & (1 << pin_num));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BOP = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BC = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);

    /* disable interrupts on the channel we want to edit (just in case) */
    EXTI->IMR &= ~(1 << pin_num);
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set callback */
    exti_ctx[pin_num].cb = cb;
    exti_ctx[pin_num].arg = arg;
    /* enable alternate function clock for the GPIO module */
    periph_clk_en(APB2, RCC_APB2ENR_AFIOEN);
    /* configure the EXTI channel */
    AFIO->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x3) * 4));
    AFIO->EXTICR[pin_num >> 2] |=  (_port_num(pin) << ((pin_num & 0x3) * 4));
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin_num);
    EXTI->RTSR |=  ((flank & 0x1) << pin_num);
    EXTI->FTSR &= ~(1 << pin_num);
    EXTI->FTSR |=  ((flank >> 1) << pin_num);
    /* active global interrupt for the selected port */
    if (pin_num < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
    /* clear event mask */
    EXTI->EMR &= ~(1 << pin_num);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin_num);
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI->IMR |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI->IMR &= ~(1 << _pin_num(pin));
}

void isr_exti(void)
{
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI->PR & EXTI->IMR);

    for (unsigned i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
        if (pending_isr & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            exti_ctx[i].cb(exti_ctx[i].arg);
        }
    }
    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */