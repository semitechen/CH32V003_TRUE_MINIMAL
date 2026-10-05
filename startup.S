.section .text.startup
    .globl _start
    .globl _estack
    .globl main

_start:
    lui  sp, %hi(_estack)
    addi sp, sp, %lo(_estack)
    call main
1:  j    1b

    .section .vector_table, "a", @progbits
    .align 4
    .globl __vectors
    .globl EXTI7_0_IRQHandler
    .globl Default_Handler

__vectors:
    .option push
    .option norvc
    j _start
    .option pop

    .rept 19
        .word Default_Handler
    .endr

    .word EXTI7_0_IRQHandler

    .rept (32 - 21)
        .word Default_Handler
    .endr

    .section .text.Default_Handler, "ax", @progbits
    .weak EXTI7_0_IRQHandler
    .set EXTI7_0_IRQHandler, Default_Handler

Default_Handler:
    j Default_Handler
