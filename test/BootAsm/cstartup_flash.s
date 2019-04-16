#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#-----------------------------------------------------------------------------
#- File source          : cstartup_flash.s
#- Object               : Boot for Final Application version to be programmed 
#-                      : in flash or ROMed.
#- Compilation flag     : None
#-
#- 1.0 06/04/00 JPP     : Creation
#- 1.1 18/08/00 JCZ     : Over-comment and optimize
#- 1.2 25/10/00 JPP     : Global for including
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#- Area Definition
#-----------------
#- Must be defined as function to put first in the code as it must be mapped
#- at offset 0 of the flash EBI_CSR0, ie. at address 0 before remap.
#------------------------------------------------------------------------------
#                AREA        reset, CODE, READONLY, INTERWORK

#------------------------------------------------------------------------------
#- Remove any semihosting support
#--------------------------------
#- The C runtime library is the IO functions provided by the semihosting.
#- They are generally costly in code and unused as the debugger is not 
#- connected to the target. 
#- Must be removed if using the embedded C library is used.
#------------------------------------------------------------------------------
#- Define "__main" to ensure that C runtime system is not linked
                .global      __main
__main:

#------------------------------------------------------------------------------
#- Define the entry point
#------------------------
#- Note on the link address and the Remap command.
#- In order to guarantee that the non position-independant code (the ARM linker 
#- armlink doesn't generate position-independant code) can work on the ARM, 
#- it must be linked at address at which it expects to run. 
#- So the -ro-base must be used to define the link address as the base 
#- address of the flash.
#- In this startup example, we use 0x100 0000 as base address. That's flash 
#- address for all AT91 Evaluation Boards.
#------------------------------------------------------------------------------
#                ENTRY
__entry:
#------------------------------------------------------------------------------
#- Exception vectors ( before Remap )
#------------------------------------
#- These vectors are read at address 0 before remap.
#- They absolutely requires to be in relative addresssing mode in order to 
#- guarantee a valid jump. For the moment, all are just looping (what may be 
#- dangerous in a final system). If an exception occurs before remap, this 
#- would result in an infinite loop. 
#- After Remap, these vectors are mapped at address 0x100 0000 and only a 
#- reset (internal or external) can make of them the actual ARM vectors.
#- Note that the infinite loop has advantage that a debugger can to show up 
#- very quickly an hardware issue during the boot sequence.
#------------------------------------------------------------------------------
                B           InitReset       /* reset */
undefvec:
                B           undefvec        /* Undefined Instruction */
swivec:
                B           swivec          /* Software Interrupt */
pabtvec:
                B           pabtvec         /* Prefetch Abort */
dabtvec: 
                B           dabtvec         /* Data Abort */
rsvdvec:
                B           rsvdvec         /* reserved */
irqvec:
                B           irqvec          /* reserved */
fiqvec:
                B           fiqvec          /* reserved */

#------------------------------------------------------------------------------
#- Exception vectors ( after Remap )
#------------------------------------
#- These vectors are read at address 0 after the remap command is performed in
#- the EBI. As they will be relocated at address 0x0 to be effective, a 
#- relative addressing is forbidden. The only possibility to get an absolute 
#- addressing for an ARM vector is to read a PC relative value at a defined
#- offset. It is easy to reserve the locations 0x20 to 0x3C (the 8 next
#- vectors) for storing the absolute exception handler address. 
#- The AIC vectoring access vectors are saved in the interrupt and fast 
#- interrupt ARM vectors. So, only 5 offsets are required ( reserved vector
#- offset is never used).
#- The provisory handler addresses are defined on infinite loop and can be 
#- modified at any time.
#- Note also that the reset is only accessible by a jump from the application 
#- to 0. It is an actual software reset.
#- As the 13 first location are used by the vectors, the read/write link 
#- address must be defined from 0x34 if internal data mapping is required.
#- (use for that the option -rw- base=0x34
#------------------------------------------------------------------------------
VectorTable:
                ldr         pc, [pc, #+0x18]          /* SoftReset */
                ldr         pc, [pc, #+0x18]          /* UndefHandler */
                ldr         pc, [pc, #+0x18]          /* SWIHandler */
                ldr         pc, [pc, #+0x18]          /* PrefetchAbortHandler */
                ldr         pc, [pc, #+0x18]          /* DataAbortHandler */
                nop                                   /* Reserved */
                ldr         pc, [pc,#-0xF20]          /* IRQ : read the AIC */
                ldr         pc, [pc,#-0xF20]          /* FIQ : read the AIC */

#- There are only 5 offsets as the vectoring is used.
                .long         SoftReset
                .long         UndefHandler
                .long         SWIHandler
                .long         PrefetchAbortHandler
                .long         DataAbortHandler
#- Vectoring Execution function run at absolut addresss
SoftReset:
                b           SoftReset
UndefHandler:
                b           UndefHandler
SWIHandler:
                b           SWIHandler
PrefetchAbortHandler:
                b           PrefetchAbortHandler
DataAbortHandler:
                b           DataAbortHandler

#------------------------------------------------------------------------------
#- EBI Initialization Data
#-------------------------
#- The EBI values depend to target choice , Clock, and memories access time.
#- Yous must be define these values in include file
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
#- From here, the code is executed from address 0. Take care, as it is linked 
#- in 0x100 0000.
#------------------------------------------------------------------------------
InitReset:

#------------------------------------------------------------------------------
#- Speed up the Boot sequence
#----------------------------
#- After reset, the number os wait states on chip select 0 is 8. All AT91 
#- Evaluation Boards fits fast flash memories, so that the number of wait 
#- states can be optimized to fast up the boot sequence.
#------------------------------------------------------------------------------
#- Load System EBI Base address and CSR0 Init Value
                ldr     r0, PtEBIBase
                ldr     r1, [pc,#-(8+.-InitTableEBI)] /* values (relative) */
    
#- Speed up code execution by disabling wait state on Chip Select 0
                str     r1, [r0]

#------------------------------------------------------------------------------
#- low level init
#--------------------------------
# Call __low_level_init to perform initialization before initializing
# AIC and calling main. 
#----------------------------------------------------------------------

                bl      __low_level_init               
                

#------------------------------------------------------------------------------
#- Reset the Interrupt Controller
#--------------------------------
#- Normally, the code is executed only if a reset has been actually performed.
#- So, the AIC initialization resumes at setting up the default vectors.
#------------------------------------------------------------------------------
#- Load the AIC Base Address and the default handler addresses
                add     r0, pc,#-(8+.-AicData)  /* @ where to read values (relative) */

                ldmia   r0, {r1-r4}

#- Setup the Spurious Vector
                str     r4, [r1, #AIC_SPU]      /* r4 = spurious handler */

#- Set up the default interrupt handler vectors
                str     r2, [r1, #AIC_SVR]      /* SVR[0] for FIQ */
                add     r1, r1, #AIC_SVR
                mov     r0, #31                 /* counter */
LoopAic1:
                str     r3, [r1, r0, LSL #2]    /* SVRs for IRQs */
                subs    r0, r0, #1              /* do not save FIQ */
                bhi     LoopAic1

                b       EndInitAic

#- Default Interrupt Handlers
AicData:
                .long   AIC_BASE                /* AIC Base Address */

#------------------------------------------------------------------------------
#- Default Interrupt Handler
#------------------------------------------------------
#- These function are defined in the AT91 library. If you want to change this 
#- you can redifine these function in your appication code
#------------------------------------------------------------------------------
                .extern  at91_default_fiq_handler
                .extern  at91_default_irq_handler
                .extern  at91_spurious_handler
PtDefaultHandler:
                .long     at91_default_fiq_handler
                .long     at91_default_irq_handler
                .long     at91_spurious_handler
EndInitAic:

#------------------------------------------------------------------------------
#- Setup Exception Vectors in Internal RAM before Remap
#------------------------------------------------------
#- That's important to perform this operation before Remap in order to guarantee
#- that the core has valid vectors at any time during the remap operation.
#- Note: There are only 5 offsets as the vectoring is used.
#------------------------------------------------------------------------------
#- Copy the ARM exception vectors

                mov     r8, #RAM_BASE_BOOT          /* @ of the hard vector in internal RAM 0x300000 */
                add     r9, pc,#-(8+.-VectorTable)  /* @ where to read values (relative) */
                ldmia   r9!, {r0-r7}            /* read 8 vectors */
                stmia   r8!, {r0-r7}            /* store them */
                ldmia   r9!, {r0-r4}            /* read 5 absolute handler addresses  */
                stmia   r8!, {r0-r4}            /* store them */

#------------------------------------------------------------------------------
#- Initialise the Memory Controller
#----------------------------------
#- That's principaly the Remap Command. Actually, all the External Bus 
#- Interface is configured with some instructions and the User Interface Image 
#- as described above. The jump "mov pc, r12" could be unread as it is after
#- located after the Remap but actually it is thanks to the Arm core pipeline.
#- The IniTableEBI addressing must be relative .
#- The PtInitRemap must be absolute as the processor jumps at this address 
#- immediatly after the Remap is performed.
#- Note also that the EBI base address is loaded in r11 by the "ldmia".
#------------------------------------------------------------------------------
#- Copy the Image of the Memory Controller
                sub     r10, pc,#(8+.-InitTableEBI) /* get the address of the chip select register image */
                ldr     r12, PtInitRemap        /* get the real jump address ( after remap ) */

#- Copy Chip Select Register Image to Memory Controller and command remap
                ldmia   r10!, {r0-r9,r11}       /* load the complete image and the EBI base */
                stmia   r11!, {r0-r9}           /* store the complete image with the remap command */

#- Jump to ROM at its new address
                mov     pc, r12                 /* jump and break the pipeline */

PtInitRemap:
                .long     InitRemap             /* address where to jump after REMAP */

#------------------------------------------------------------------------------
#- The Reset Handler after Remap
#-------------------------------
#- From here, the code is executed from its link address, ie. 0x100 0000.
#------------------------------------------------------------------------------
InitRemap:

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

.equ	IRQ_STACK_SIZE,       (3*8*4)     /* 3 words per interrupt priority level */
.equ	FIQ_STACK_SIZE,       (3*4)       /* 3 words */
.equ	ABT_STACK_SIZE,       (1*4)       /* 1 word  */
.equ	UND_STACK_SIZE,       (1*4)       /* 1 word  */
                                          
#------------------------------------------------------------------------------
#- Top of Stack Definition
#-------------------------
#- Fast Interrupt, Interrupt, Abort, Undefined and Supervisor Stack are located
#- at the top of internal memory in order to speed the exception handling 
#- context saving and restoring.
#- User (Application, C) Stack is located at the top of the external memory.
#------------------------------------------------------------------------------

.equ	TOP_EXCEPTION_STACK,         RAM_LIMIT           /* Defined in part */
.equ	TOP_APPLICATION_STACK,       EXT_SRAM_LIMIT      /* Defined in Target */

#------------------------------------------------------------------------------
#- Setup the stack for each mode
#-------------------------------
                ldr     r0, =TOP_EXCEPTION_STACK

#;- Set up Fast Interrupt Mode and set FIQ Mode Stack
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
                msr     CPSR_c, #ARM_MODE_UNDEF | I_BIT| F_BIT
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
                msr     CPSR_c, #ARM_MODE_SYS      /* set User mode	*/
                ldr     r13, =TOP_APPLICATION_STACK /* Init stack User  */
                
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
                .extern       Image_RO_Limit      	/* End of ROM code (=start of ROM data) */
                .extern       Image_RW_Base       	/* Base of RAM to initialise */           
                .extern       Image_ZI_Base       	/* Base and limit of area */              
                .extern       Image_ZI_Limit      	/* to zero initialise */  
                .extern       Image_ROM_Base
                .extern       Image_RO_Base

                ldr         r0, =Image_ROM_Base 	/* Get pointer to RAM CODE START */
                ldr         r1, =Image_RO_Base  	/*Get pointer to ROM BASE */
                ldr         r3, =Image_RO_Limit   	/* Get pointer to (ROM CODE + ROM ONLY READ DATA) LIMIT */

                add        r3,r3,#4
LoopRO:         cmp         r0, r3                  /* Copy ROM CODE + ROM ONLY READ DATA*/
                ldrcc       r2, [r0], #4
                strcc       r2, [r1], #4
                bcc         LoopRO


                ldr         r0, =Image_RO_Limit 	/* Get pointer to (ROM CODE + ROM ONLY READ DATA) LIMIT = ROM RW DATA */
                ldr         r1, =Image_RW_Base  	/* Get pointer to RAM DATA START*/
                ldr         r3, =Image_ZI_Base  	/* Zero init base => top of initialised data */                
                
                cmp         r0, r1                  /* Check that they are different */
                beq         NoRW
LoopRw:         cmp         r1, r3                  /* Copy init data */
                ldrcc       r2, [r0], #4
                strcc       r2, [r1], #4
                bcc         LoopRw
NoRW:           ldr         r1, =Image_ZI_Limit 	/* Top of zero init segment */
                mov         r2, #0
LoopZI:         cmp         r3, r1                  /* Zero init */
                strcc       r2, [r3], #4
                bcc         LoopZI
                
                


#------------------------------------------------------------------------------
#- Branch on C code Main function (with interworking)
#----------------------------------------------------
#- Branch must be performed by an interworking call as either an ARM or Thumb 
#- main C function must be supported. This makes the code not position-
#- independant. A Branch with link would generate errors 
#------------------------------------------------------------------------------
                .extern      Main

                ldr         r0, =Main
                ldr         r1,=Image_ROM_Base/* Get MAIN point */
                ldr         r2,=Image_RO_Base
                add       r0,r0,r2
                sub       r0,r0,r1
                mov         lr, pc
                bx          r0

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
