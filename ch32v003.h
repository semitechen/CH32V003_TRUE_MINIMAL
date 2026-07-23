#pragma once
#include "include/reg_access.h"
#include "include/rcc.h"
#include "include/gpio.h"
#include "include/afio.h"
#include "include/exti.h"
#include "include/pfic.h"
#include "include/irq.h"
#include "include/flash.h"

/* Composite helper for EXTI setup */
#define SETUP_EXTI_PIN(pin, portcode, rise, fall, irqnum) do { \
	AFIO_CONFIG_EXTI((pin), (portcode));                      \
	if (rise) EXTI_RTENR |= (1U << (pin));                    \
	if (fall) EXTI_FTENR |= (1U << (pin));                    \
	EXTI_INTENR |= (1U << (pin));                             \
	PFIC_IENR0 = (1U << (irqnum));                            \
} while (0)
