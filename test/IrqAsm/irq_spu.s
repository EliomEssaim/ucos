#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#------------------------------------------------------------------------------
#- File source          : irq_spu.s
#- Object               : Spurious Interrupt Handler.
#-
#- 1.0 01/04/00 JCZ     : Creation
#- 1.1 02/11/00 JPP     : Green Hills
#------------------------------------------------------------------------------

#            AREA        AT91Lib, CODE, READONLY, INTERWORK

            .INCLUDE     "../include/At91M55800.inc"

#------------------------------------------------------------------------------
#- Function             : at91_spurious_handler
#- Treatments           : Simplest and Fastest Spurious Interrupt Handler.
#- Input Parameters     : r14 = lr = interrupt instruction address + 4
#-                      : SPSR = CPSR when core interrupted
#- Output Parameters    : None
#- Registers lost       : None
#- Called Functions     : None
#- Called Macros        : None
#------------------------------------------------------------------------------
            .global      at91_spurious_handler

at91_spurious_handler:
            sub         r14, r14, #4                /* Adjust LR */
            stmdb       r13!, {r14}                 /* Save LR */
            ldr         r14, =AIC_BASE              /* Mark the End of Interrupt */
            str         r14, [r14, #AIC_EOICR]
            ldmia       r13!, {pc}^                 /* Restore adjusted */

#            END
