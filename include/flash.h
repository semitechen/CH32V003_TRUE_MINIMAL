#pragma once
#include "reg_access.h"

/* -------- Flash Memory Interface (FPEC) -------- */
#define FLASH_BASE      0x40022000UL

/* Register Offsets */
#define FLASH_KEYR_OFFSET      0x04
#define FLASH_OBKEYR_OFFSET    0x08
#define FLASH_STATR_OFFSET     0x0C
#define FLASH_CTLR_OFFSET      0x10
#define FLASH_ADDR_OFFSET      0x14
#define FLASH_OBR_OFFSET       0x1C

/* Register Access */
#define FLASH_KEYR      REG(FLASH_BASE + FLASH_KEYR_OFFSET)
#define FLASH_OBKEYR    REG(FLASH_BASE + FLASH_OBKEYR_OFFSET)
#define FLASH_STATR     REG(FLASH_BASE + FLASH_STATR_OFFSET)
#define FLASH_CTLR      REG(FLASH_BASE + FLASH_CTLR_OFFSET)
#define FLASH_ADDR      REG(FLASH_BASE + FLASH_ADDR_OFFSET)
#define FLASH_OBR       REG(FLASH_BASE + FLASH_OBR_OFFSET)

/* Unlock Keys */
#define FLASH_KEY1      0x45670123
#define FLASH_KEY2      0xCDEF89AB

/* Option Byte Address */
#define OB_USER_ADDR    0x1FFFF800

/* Bit Definitions */
#define FLASH_CTLR_LOCK    (1U << 7)
#define FLASH_CTLR_OBWRE   (1U << 9)
#define FLASH_CTLR_OBPG    (1U << 4)
#define FLASH_CTLR_STRT    (1U << 6)
#define FLASH_STATR_BSY    (1U << 0)
#define FLASH_STATR_EOP    (1U << 5)

#define FLASH_CTLR_OBER    (1U << 5)

/* * Sets PD7 as GPIO by programming User Option Bytes.
 * This disables hardware reset. Chip erasure may be required to revert.
 * This macro performs the full unlock and program sequence.
 */
#define FLASH_SET_NRST_AS_GPIO() do { \
    /* Check if NRST is already GPIO (Bit 4:3 == 11) to avoid wear */ \
    /* USER option byte is at 0x1FFFF802 */ \
    uint16_t current_user = ((volatile uint16_t *)0x1FFFF802)[0]; \
    if ((current_user & 0x18) != 0x18) { \
        /* 1. Unlock Flash if locked */ \
        if (FLASH_CTLR & FLASH_CTLR_LOCK) { \
            FLASH_KEYR = FLASH_KEY1; \
            FLASH_KEYR = FLASH_KEY2; \
        } \
        /* 2. Unlock Option Bytes if locked */ \
        if (!(FLASH_CTLR & FLASH_CTLR_OBWRE)) { \
            FLASH_OBKEYR = FLASH_KEY1; \
            FLASH_OBKEYR = FLASH_KEY2; \
        } \
        while(FLASH_STATR & FLASH_STATR_BSY); \
        /* 3. Erase Option Bytes (This clears RDPR, USER, Data0, Data1) */ \
        FLASH_CTLR |= FLASH_CTLR_OBER; \
        FLASH_CTLR |= FLASH_CTLR_STRT; \
        while(FLASH_STATR & FLASH_STATR_BSY); \
        FLASH_CTLR &= ~FLASH_CTLR_OBER; \
        /* 4. Program Option Bytes */ \
        FLASH_CTLR |= FLASH_CTLR_OBPG; \
        /* Write RDPR (0x1FFFF800) back to 0xA5 to keep chip unlocked */ \
        ((volatile uint16_t *)0x1FFFF800)[0] = 0xA5; \
        while(FLASH_STATR & FLASH_STATR_BSY); \
        /* Write USER (0x1FFFF802) with RST_MODE set to 11 (0x18) */ \
        ((volatile uint16_t *)0x1FFFF802)[0] = ((current_user & 0x00FF) | 0x18); \
        while(FLASH_STATR & FLASH_STATR_BSY); \
        /* Disable Programming and Lock */ \
        FLASH_CTLR &= ~FLASH_CTLR_OBPG; \
        FLASH_CTLR |= FLASH_CTLR_LOCK; \
        /* 5. System Reset for Option Bytes to take effect immediately */ \
        /* PFIC_CFGR (0xE000E048) bit 7 triggers software reset */ \
        REG(0xE000E048) |= (1U << 7); \
    } \
} while(0)
