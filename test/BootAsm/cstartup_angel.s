#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#-----------------------------------------------------------------------------
#- File source          : cstartup_angel.s
#- Object               : Specific Startup for Angel compatibility to must be 
#-                        loaded in SRAM. And support the Semihosting
#- Compilation flag     : SEMIHOSTING => use the semihosting facilities
#-
#- 1.0 06/04/00 JPP     : Creation
#- 1.1 18/08/00 JCZ     : Over-comment and optimize
#- 1.2 25/10/00 JPP     : Global for including
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#- Area Definition
#-----------------
#- Must be defined as function to put first in the code as it must be mapped
#- at SRAM.
#------------------------------------------------------------------------------
#                AREA        reset, CODE, READONLY, INTERWORK

#------------------------------------------------------------------------------
#- Semihosting support
#--------------------------------
#- The C runtime library is the IO functions provided by the semihosting.
#- They are generally costly in code and can be used as the debugger 
#------------------------------------------------------------------------------
#- Define "__main" to ensure that C runtime system is not linked
                .global      __main
__main:

#------------------------------------------------------------------------------
#- Define the entry point
#------------------------
#------------------------------------------------------------------------------
#                ENTRY
__entry:
#------------------------------------------------------------------------------
#- Exception vectors ( Remap is managed by Angel )
#------------------------------------------------------------------------------
                B           InitReset       /* reset */

#------------------------------------------------------------------------------
#- EBI Initialization Data
#-------------------------
#- The EBI User Interface Image which is copied by the boot.
#- The EBI_CSR_x are defined in the target and hardware depend.
#- That's hardware! Details in the Electrical Datasheet of the AT91 device.
#- EBI Base Address is added at the end for commodity in copy code.
#------------------------------------------------------------------------------
InitTableEBI:
            .long         EBI_CSR_0  
            .long         EBI_CSR_1  
            .long         EBI_CSR_2  
            .long         EBI_CSR_3  
            .long         EBI_CSR_4  
            .long         EBI_CSR_5  
            .long         EBI_CSR_6  
            .long         EBI_CSR_7  
            .long         0x00000001  /* REMAP command */
            .long         0x00000006  /* 6 memory regions, standard read */
PtEBIBase:
            .long         EBI_BASE    /* EBI Base Address */

#------------------------------------------------------------------------------
#- The reset handler before Remap
#--------------------------------
#- From here, the code is executed from SRAM address
#------------------------------------------------------------------------------
InitReset:

#----------------------------------------------------------------------
# Call __low_level_init to perform initialization before initializing
# AIC and calling main. 
#----------------------------------------------------------------------

                bl      __low_level_init

#------------------------------------------------------------------------------
#- Initialise the Memory Controller
#----------------------------------
#- The IniTableEBI addressing must be relative .
#- The PtInitRemap must be absolute as the processor jumps at this address 
#- Note also that the EBI base address is loaded in r11 by the "ldmia".
#------------------------------------------------------------------------------
#- Copy the Image of the Memory Controller
                sub     r10, pc,#(8+.-InitTableEBI) /* get the address of the chip select register image */
#- Copy Chip Select Register Image to Memory Controller and command remap
                ldmia   r10!, {r0-r9,r11}       /* load the complete image and the EBI base */
                stmia   r11!, {r0-r9}           /* store the complete image with the remap command */

#------------------------------------------------------------------------------
#- Stack Sizes Definition
#------------------------
#- Interrupt Stack requires 3 words x 8 priority level x 4 bytes when using
#- the vectoring. This assume that the IRQ_ENTRY/IRQ_EXIT macro are used. 
#- The Interrupt Stack must be adjusted depending on the interrupt handlers.
#- Fast Interrupt is the same than Interrupt without priority level.
#- Other stacks are defined by default to save one word each.
#- The System stack size is not defined and is limited by the free internal 
#- SRAM. 
#- User stack size is not defined and is limited by the free external SRAM.
#------------------------------------------------------------------------------

.equ	IRQ_STACK_SIZE,      (3*8*4)     /* 3 words per interrupt priority level */
.equ	FIQ_STACK_SIZE,      (3*4)       /* 3 words */
.equ	ABT_STACK_SIZE,      (1*4)       /* 1 word */
.equ	UND_STACK_SIZE,      (1*4)       /* 1 word */

#------------------------------------------------------------------------------
#- Top of Stack Definition
#-------------------------
#- Fast Interrupt, Interrupt, Abort, Undefined and Supervisor Stack are located
#- at the top of internal memory in order to speed the exception handling 
#- context saving and restoring.
#- User (Application, C) Stack is located at the top of the external memory.
#------------------------------------------------------------------------------

.equ	TOP_EXCEPTION_STACK,          RAM_LIMIT           /* Defined in part */
.equ	TOP_APPLICATION_STACK,        EXT_SRAM_LIMIT      /* Defined in Target */

#------------------------------------------------------------------------------
#- Setup the stack for each mode
#-------------------------------
                ldr     r0, =TOP_EXCEPTION_STACK

#- Set up Fast Interrupt Mode and set FIQ Mode Stack
                msr     CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack FIQ */
                sub     r0, r0, #FIQ_STACK_SIZE

#- Set up Interrupt Mode and set IRQ Mode Stack
                msr     CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack IRQ */
                sub     r0, r0, #IRQ_STACK_SIZE

#- Set up Abort Mode and set Abort Mode Stack
                msr     CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack Abort */
                sub     r0, r0, #ABT_STACK_SIZE

#- Set up Undefined Instruction Mode and set Undef Mode Stack
                msr     CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack Undef */
                sub     r0, r0, #UND_STACK_SIZE

#- Set up Supervisor Mode and set Supervisor Mode Stack
                msr     CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack Sup */

#------------------------------------------------------------------------------
#- Setup Application Operating Mode and Enable the interrupts
#------------------------------------------------------------
#- System Mode is selected first and the stack is setup. This allows to prevent 
#- any interrupt occurence while the User is not initialized. System Mode is
#- used as the interrupt enabling would be avoided from User Mode (CPSR cannot
#- be written while the core is in User Mode).
#------------------------------------------------------------------------------
                msr     CPSR_c, #ARM_MODE_USER      /* set User mode */
                ldr     r13, =TOP_APPLICATION_STACK /* Init stack User */
                
#------------------------------------------------------------------------------
#- Initialise C variables
#------------------------
#- Following labels are automatically generated by the linker. 
#- RO: Read-only = the code
#- RW: Read Write = the data pre-initialized and zero-initialized.
#- ZI: Zero-Initialized.
#- Pre-initialization values are located after the code area in the image.
#- Zero-initialized datas are mapped after the pre-initialized.
#- Note on the Data position : 
#- If using the ARMSDT, when no -rw-base option is used for the linker, the 
#- data area is mapped after the code. You can map the data either in internal
#- SRAM ( -rw-base=0x40 or 0x34) or in external SRAM ( -rw-base=0x2000000 ).
#- Note also that to improve the code density, the pre_initialized data must 
#- be limited to a minimum.
#------------------------------------------------------------------------------
                .extern      Image_RO_Limit      /* End of ROM code (=start of ROM data) */
                .extern      Image_RW_Base       /* Base of RAM to initialise */
                .extern      Image_ZI_Base       /* Base and limit of area */
                .extern      Image_ZI_Limit      /* to zero initialise */

                ldr         r0, =Image_RO_Limit /* Get pointer to ROM data */
                ldr         r1, =Image_RW_Base  /* and RAM copy */
                ldr         r3, =Image_ZI_Base  /* Zero init base => top of initialised data */
                cmp         r0, r1              /* Check that they are different */
                beq         NoRW
LoopRw:         cmp         r1, r3              /* Copy init data */
                ldrcc       r2, [r0], #4
                strcc       r2, [r1], #4
                bcc         LoopRw
NoRW:           ldr         r1, =Image_ZI_Limit /* Top of zero init segment */
                mov         r2, #0
LoopZI:         cmp         r3, r1              /* Zero init */
                strcc       r2, [r3], #4
                bcc         LoopZI


        .IFDEF SEMIHOSTING

#;- Branch on Entry point
#;- ---------------------
#;- Allows semihosting initialisation
#;- you must be set the semihosting debugger for ARM SDT :$top_of_memory = 0x 
#;- for Aspex  :top_memory = 0x     in bcd board description

                .extern      __entry

.equ	SEMIHOSTING_STACK_SIZE,  (8*1024)                 

                ldr         r0, = SEMIHOSTING_STACK_SIZE
                sub         r13, r13,r0
                
                b           __entry
        .ELSE                                /* not use SEMIHOSTING */
#------------------------------------------------------------------------------
#- Branch on C code Main function (with interworking)
#----------------------------------------------------
#- Branch must be performed by an interworking call as either an ARM or Thumb 
#- main C function must be supported. This makes the code not position-
#- independant. A Branch with link would generate errors 
#------------------------------------------------------------------------------
                .extern      main

                ldr         r0, =main
                mov         lr, pc
                bx          r0
        .ENDIF                               /* endif SEMIHOSTING */
                
#------------------------------------------------------------------------------
#- Loop for ever
#---------------
#- End of application. Normally, never occur.
#- Could jump on Software Reset ( B 0x0 ).
#------------------------------------------------------------------------------
End:
                b           End

                .global     __gccmain
__gccmain:
				mov     	pc, lr   
                
#            END
