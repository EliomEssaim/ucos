#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#-----------------------------------------------------------------------------
#- File source          : cstartup.s
#- Object               : C startup choice
#-
#- 1.0 17/07/00  PF     : Creation
#- 1.1 31/10/00 JPP     : New cstartup
#------------------------------------------------------------------------------
                 .INCLUDE     "../include/MyBoard.inc"

#------------------------------------------------------------------------------
#               Angel Rom Monitor Method
#- Preprocessor Flag can be set : SEMIHOSTING
#------------------------------------------------------------------------------
                .IFDEF		AT91_DEBUG_ANGEL       	/* { */
                .INCLUDE    "../BootAsm/cstartup_angel.s"

#----------------------------------------------------------------------
# Call __low_level_init to perform initialization before initializing
# AIC and calling main.
# Diasable all peripherial clock
#----------------------------------------------------------------------
__low_level_init:
                mov         pc,r14              	/* Return */
                .ENDIF                           	/* AT91_DEBUG_ANGEL } */

#------------------------------------------------------------------------------
#               ICE or Sram Method
#- Preprocessor Flag can be set : SEMIHOSTING
#------------------------------------------------------------------------------

                .IFDEF		AT91_DEBUG_ICE      	/* { */
                .INCLUDE    "../BootAsm/cstartup_ice.s"
#----------------------------------------------------------------------
# Call __low_level_init to perform initialization before initializing
# AIC and calling main.
# Diasable all peripherial clock
#----------------------------------------------------------------------
__low_level_init:
                mvn         r0,#0                /* R0<- 0xFFFFFFFF */
                ldr         r1,=APMC_BASE        /* Get Power saving configuartion */
                str         r0,[r1,#APMC_PCDR]   /* Diasable all peripherial clock */
                mov         pc,r14               /* Return */

                .ENDIF                           /* AT91_DEBUG_ICE  } */

#------------------------------------------------------------------------------
#               Flash Method
#------------------------------------------------------------------------------

                .IFDEF		AT91_DEBUG_NONE        	/* { */
                .INCLUDE    "../BootAsm/cstartup_flash.s"
#----------------------------------------------------------------------
# Call __low_level_init to perform initialization before initializing
# AIC and calling main.
# Pll Initialization 
#----------------------------------------------------------------------
__low_level_init:

#- Speed up the System Frequency.
#---------------------------------
                ldr     r0, =0x002F0002             /* MOSCEN = 1, OSCOUNT = 47  (1.4ms/30祍)			*/
                ldr     r1, =APMC_BASE              /* Get the APMC Base Address                        */
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */

#- Reading the APMC Status register to detect when the oscillator is stabilized
#------------------------------------------------------------------------------
                mov     r4, #APMC_MOSCS
                
MoscsLoop:
                ldr     r2, [r1, #APMC_SR]
                and     r2, r4,r2
                cmp     r2, #APMC_MOSCS
                bne     MoscsLoop               
                

#- Commuting from Slow Clock to Main Oscillator (16Mhz)
#------------------------------------------------------
                ldr     r0, =0x002F4002             /* MOSCEN = 1, OSCOUNT = 47  (1.4ms/30祍)			*/
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */
               
/*英蓓特开发板需要使用PLL输出作为主频*/                
.IFDEF EMBEST_BOARD                

#-Setup the PLL
#---------------

                ldr     r0, =0x032F4102             /* MUL = 1, PLLCOUNT = 3, CSS = 1					*/
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */

#- Reading the APMC Status register to detect when the PLL is stabilized
#-----------------------------------------------------------------------
                mov     r4, #APMC_PLL_LOCK
Pll_Loop:
                ldr     r3, [r1, #APMC_SR]
                and     r3, r4,r3
                cmp     r3, #APMC_PLL_LOCK
                bne     Pll_Loop

#- Commuting from 16Mhz to PLL @ 32MHz
#--------------------------------------
                ldr     r0, =0x032F8102             /* CSS = 2, MUL = 1									*/
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */

#- Now the Master clock is the output of PLL @ 32MHz
#----------------------------------------------------

.ENDIF
/*.IFDEF EMBEST_BOARD*/


/*2006年监控开发板需要使用PLL输出作为主频*/                
.IFDEF OUR_BOARD                

#-Setup the PLL
#---------------

                ldr     r0, =0x032F4102             /* MUL = 1, PLLCOUNT = 3, CSS = 1					*/
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */

#- Reading the APMC Status register to detect when the PLL is stabilized
#-----------------------------------------------------------------------
                mov     r4, #APMC_PLL_LOCK
Pll_Loop:
                ldr     r3, [r1, #APMC_SR]
                and     r3, r4,r3
                cmp     r3, #APMC_PLL_LOCK
                bne     Pll_Loop

#- Commuting from 16Mhz to PLL @ 32MHz
#--------------------------------------
                ldr     r0, =0x032F8102             /* CSS = 2, MUL = 1									*/
                str     r0, [r1, #APMC_CGMR]        /* Store the configuration of the Clock Generator   */

#- Now the Master clock is the output of PLL @ 32MHz
#----------------------------------------------------

.ENDIF
/*.IFDEF OUR_BOARD*/

                mov         pc,r14              	/* Return				*/
                .ENDIF                           	/* AT91_DEBUG_NONE }	*/


#        END
