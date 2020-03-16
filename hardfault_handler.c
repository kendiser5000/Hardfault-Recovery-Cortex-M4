/**
 * @file hardfault_handler.c
 * @author Sikender Ashraf
 * @brief Example hardfault recovery handler to help with debugging
 * @version 0.1
 * @date 2020-03-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdint.h>
#include <stdio.h>

#define DUMP_STACK 
#define TRY_RECOVER

#define DUMP(...) printf

/** These defines are from Interrupt MemFault to determine whcih stack is in use **/
/** https://interrupt.memfault.com/blog/cortex-m-fault-debug#registers-prior-to-exception **/
#define  HARDFAULT_CHECK_STACK(_x) \
  __asm volatile(              \
        "tst lr, #4           \n"\
        "ite eq               \n"\
        "mrseq r0, msp        \n"\
        "mrsne r0, psp        \n"\
        "b User_HardFault_Handler \n"\
  )

/** Src: https://interrupt.memfault.com/blog/cortex-m-fault-debug#registers-prior-to-exception **/
#define HALT_IF_DEBUGGING()                              \
  do {                                                   \
    if ((*(volatile uint32_t *)0xE000EDF0) & (1 << 0)) { \
      __asm("bkpt 1");                                   \
    }                                                    \
  } while (0)                                        




#define ATTEMPT_RECOVERY() \
    __asm volatile(                         \
        "mov32   r3, #0xE000ED29            ;get SCB BFFSR 0xE000ED29 for FSR \n"\
        "ldrb    r0, [r3, #0]               \n"\
        ";check if it is possible to recover \n"\
        "tst     r0, #0x2         	        ;check if it is a Precise ERR (pc points to fault)\n"\
        "beq     _check_instruction_error   \n"\
        "ldr     r3, [sp, #0x20] 	        ;get PC from stack\n"\
        "ldrh    r0, [r3, #0]    	        \n"\
        "add     r3, r3, #2                 \n"\
        "and     r0, r0, #0xf800            \n"\
        "cmp     r0, #0xf800                \n"\
        "ITTT    ne                         \n"\
        "cmpne   r0, #0xf000     	        \n"\
        "cmpne   r0, #0xe800                \n"\
        "bne     _get_return_address        \n"\
        "adds    r3, #2                     \n"\
        \
        "_get_return_address                \n"\
        "   str r3, [sp, #0x20]             ;Put PC+2 back into stack for LR\n"\
        "   b   _end_hardfault              \n"\
        \
        "_check_instruction_error           ;causes: Branch to invalid mem, invalid ret due to corrupt stack, Incorrect entry in the exception vector table\n"\
        "   tst r0, #0x01                   ;check if it is a BUSERR, instruction bus error\n"\
        "   beq _end_hardfault              \n"\
        "   ldr    r3, [sp, #0x1c]          ;BFAR does not write fault register\n"\
        "   str    r3, [sp, #0x20]          ;Must get PC from stack\n"\
        "   b _end_hardfault                \n"\
        \
        "_end_hardfault                     \n"\
        "   mov32    r3, #0xE000ED29        ;0xE000ED29 for FSR\n"\
        "   movw r0, #0x3                   \n"\
        "   strb r3, [r0, #0]               \n"\
        "BX LR                              \n"\
    )


/**
 * @struct StackFrame
 */
typedef struct __attribute__((packed)) StackFrame {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t RET_ADDR;
  uint32_t XPSR;
} StackFrame_t;




/**
 * @brief User defined hardfault handler
 * 
 */
void User_HardFault_Handler(
    #ifdef DUMP_STACK 
    StackFrame_t* stack 
    #endif
    )
{
    #ifdef DUMP_STACK
    DUMP("Register R0 value: 0x%32x", stack.R0);
    DUMP("Register R1 value: 0x%32x", stack.R1);
    DUMP("Register R2 value: 0x%32x", stack.R2);
    DUMP("Register R3 value: 0x%32x", stack.R3);
    DUMP("Register R12 value: 0x%32x", stack.R12);
    DUMP("Register LR value: 0x%32x", stack.LR);
    DUMP("Register RET_ADDR value: 0x%32x", stack.RET_ADDR);
    DUMP("Register XPSR value: 0x%32x", stack.XPSR);
    #endif

    #ifdef TRY_RECOVER
    ATTEMPT_RECOVERY();
    #endif

    HALT_IF_DEBUGGING();
}


/**
 * @brief User defined hardfault handler
 * 
 */
void HardFault_Handler(void)
{
    #ifdef DUMP_STACK
    __asm(                     \
    "tst lr, #4             \n"\
    "ite eq                 \n"\
    "mrseq r0, msp          \n"\
    "mrsne r0, psp          \n"\
    "b User_HardFault_Handler   \n"\
    );
    #else
    User_HardFault_Handler();   
    #endif

    // user defined code here
    while(1){};
    

}







