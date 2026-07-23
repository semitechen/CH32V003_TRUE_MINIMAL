#pragma once
#include "reg_access.h"

/* -------- Alternate Function I/O (AFIO) -------- */
#define AFIO_BASE        0x40010000UL
#define AFIO_EXTICR_REG  REG(AFIO_BASE + 0x08)

/* Port mapping codes */
#define AFIO_EXTICR_PA  0x0
#define AFIO_EXTICR_PC  0x2
#define AFIO_EXTICR_PD  0x3

#define AFIO_CONFIG_EXTI(pin, portcode) do {                \
	uint32_t tmp = AFIO_EXTICR_REG;                          \
	tmp &= ~(0x3U << ((pin) * 2));                           \
	tmp |= ((uint32_t)((portcode) & 0x3U) << ((pin) * 2));   \
	AFIO_EXTICR_REG = tmp;                                   \
} while (0)

 
/* Register Offsets */
#define AFIO_PCFR1_OFFSET 0x04

/* Register Access */
#define AFIO_PCFR1 REG(AFIO_BASE + AFIO_PCFR1_OFFSET)

/* SWD Configuration Bits (SWCFG[2:0] at bits 26:24) */
/* 000: SWD Enabled (Default) */
/* 100: SWD Disabled (GPIO)   */
#define AFIO_PCFR1_SWCFG_MASK (0x7U << 24)
#define AFIO_PCFR1_SWCFG_GPIO (0x4U << 24)

/* * Disable SWD to use PD1 as GPIO.
 * WARNING: You will lose debug access immediately. 
 * Ensure RCC_AFIOEN is enabled before calling this.
 */
#define AFIO_DISABLE_SWDIO() do { \
    AFIO_PCFR1 &= ~AFIO_PCFR1_SWCFG_MASK; \
    AFIO_PCFR1 |= AFIO_PCFR1_SWCFG_GPIO;  \
} while(0)
