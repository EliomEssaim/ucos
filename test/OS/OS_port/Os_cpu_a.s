#********************************************************************************************************
#                                               uC/OS-II
#                                         The Real-Time Kernel
#
#                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
#                                          All Rights Reserved
#
# File : OS_CPU_A.ASM
# By   : Jean J. Labrosse
#********************************************************************************************************

#********************************************************************************************************
#                                               ARM Port
#
#                 Target           : ARM (Includes ARM7, ARM9)
#                 Ported by        : Michael Anburaj
#                 URL              : http://geocities.com/michaelanburaj/    Email : michaelanburaj@hotmail.com
#
#********************************************************************************************************



        .text

#*********************************************************************************************************
#                                          START MULTITASKING
#                                       void OSStartHighRdy(void)
#
# Note : OSStartHighRdy() MUST:
#           a) Call OSTaskSwHook() then,
#           b) Set OSRunning to TRUE,
#           c) Switch to the highest priority task.
#*********************************************************************************************************

	.IFDEF AT91M55800
    .include "../Include/At91M55800.inc"
    #.INCLUDE "E:/src/ourboard/Include/MyBoard.inc"
#--------------------------------
#- Advanced Interrupt Controller
#--------------------------------

    .EQU	TC0_BASE,      TCB0_BASE      /* Channel 0 Base Address */
	.ENDIF

        .EXTERN  OSTaskSwHook
        .EXTERN  OSRunning
        .EXTERN  OSTCBHighRdy

        .global  OSStartHighRdy

OSStartHighRdy:  #第一个任务运行 的切换 目的：获得堆栈让它弹栈 没有旧任务 需要保存旧任务现场

        bl OSTaskSwHook             @ Call user defined task switch hook

        ldr r4,=OSRunning           @ Indicate that multitasking/OS has started
        mov r5,#1
        strb r5,[r4]

        ldr r4,=OSTCBHighRdy        @ Get highest priority task TCB address 指向指针的指针

        ldr r4,[r4]                 @ get stack pointer 变成指针
        ldr sp,[r4]                 @ switch to the new stack tcb的第一个地址是任务的任务栈栈顶

        ldmfd sp!,{r4}              @ pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              @ pop new task's psr
        msr CPSR_cxsf,r4
        ldmfd sp!,{r0-r12,lr,pc}    @ pop new task's r0-r12,lr & pc

#*********************************************************************************************************
#                                PERFORM A CONTEXT SWITCH (From task level)
#                                           void OSCtxSw(void)
#
# Note(s):    Upon entry, 
#             OSTCBCur     points to the OS_TCB of the task to suspend
#             OSTCBHighRdy points to the OS_TCB of the task to resume
#
#*********************************************************************************************************

        .EXTERN  OSTCBCur
        .EXTERN  OSTaskSwHook
        .EXTERN  OSTCBHighRdy
        .EXTERN  OSPrioCur
        .EXTERN  OSPrioHighRdy

        .global  OSCtxSw

OSCtxSw:#任务切换 schedu() 用的 中断有关的地方有四个 系统刚开始 中断无关 中断相关（定时器以及外部中断） OSPrioHighRdy是ID //找到函数的定位 站在哪里 讲清楚铺垫
#与schedu（）搭配完成 调度切换的任务 OS运行在管理模式 app运行在系统模式  
# Special optimised code below:
        stmfd sp!,{lr}              @ push pc (lr should be pushed in place of PC)lr是什么本模式的lr （任务跳转时会做）1、内容相同吗？（schedu的下一行）2、
        stmfd sp!,{r0-r12,lr}       @ push lr & register file
        mrs r4,cpsr
        stmfd sp!,{r4}              @ push current psr
        mrs r4,spsr
        stmfd sp!,{r4}              @ push current spsr

        @ OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy#复制OSPrioCur和OSPrioHighRdy
        ldrb r6,[r5]
        strb r6,[r4]
        
        @ Get current task TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]#r5就是OSTCBCur的地址
        str sp,[r5]                 @ store sp in preempted tasks's TCB#保存旧任务的sp到自己的tcb中

        bl OSTaskSwHook             @ call Task Switch Hook

        @ Get highest priority task TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr sp,[r6]                 @ get new task's stack pointer

        @ OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 @ set new current task TCB address

        ldmfd sp!,{r4}              @ pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              @ pop new task's psr
        msr CPSR_cxsf,r4
        ldmfd sp!,{r0-r12,lr,pc}    @ pop new task's r0-r12,lr & pc


#*********************************************************************************************************
#                                PERFORM A CONTEXT SWITCH (From an ISR)
#                                        void OSIntCtxSw(void)
#
# Note(s): This function only flags a context switch to the ISR Handler
#
#*********************************************************************************************************

        .EXTERN  OSIntCtxSwFlag

        .global  OSIntCtxSw

OSIntCtxSw:#中断中退出的时的切换

        #OSIntCtxSwFlag = True
        ldr r0,=OSIntCtxSwFlag#全局变量 这里仅仅置1 置一个flag来控制
        mov r1,#1
        str r1,[r0]
        mov pc,lr#返回
        

#*********************************************************************************************************
#                                            IRQ HANDLER
#
#        This handles all the IRQs
#        Note: FIQ Handler should be written similar to this
#
#*********************************************************************************************************

        .EXTERN  Timer0Interrupt
        .EXTERN  OSIntEnter
        .EXTERN  OSIntExit

        .EXTERN  OSIntCtxSwFlag
        .EXTERN  OSTCBCur
        .EXTERN  OSTaskSwHook
        .EXTERN  OSTCBHighRdy
        .EXTERN  OSPrioCur
        .EXTERN  OSPrioHighRdy

		.equ NOINT, 0xc0

        .global  OSTickISR
OSTickISR:
	    stmfd sp!,{r0-r12,lr}

#这里的lr 需要再保存到这个任务堆栈的上 因为各个任务共用一个模式 lr即使保存到连接寄存器（保护模式的现场） 回来的时候还是需要再次确定位置 保存a任务的现场
#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
        ldr     r14, =AIC_BASE
        str     r14, [r14, #AIC_IVR]
    
# read the interrupt status reg to clear it 
        ldr     r12,=TC0_BASE       	/* load tc0  base address			*/
        ldr     r12,[r12, #0x020]   	/* read from status register offse  */

#- Enable Interrupt and Switch in SYS Mode
#        mrs     r12, CPSR
#        bic     r12, r12, #I_BIT
#        orr     r12, r12, #ARM_MODE_SYS
#        msr     CPSR_c, r12
        
#- Save scratch/used registers and LR in SYS Stack
#        stmfd   sp!, { r0-r12,r14}
	            	
        ldr     r12, =OSIntEnter         /*让嵌套计数器加一*/
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
        ldr     r12, =OSTimeTick         /*idle一定是最后一个任务*/
        mov     r14,pc                   
        bx      r12                     /* Branch to OsTimeTick */      
        ldr     r12, =OSIntExit          
        mov     r14,pc                   
        bx      r12                     /* Branch to OSIntExit 	*/

# Interrupt Exit if no higher priority task ready to run        

# restore interrupted task  
#- Restore scratch/used registers and LR from System Stack
#        ldmfd   sp!, { r0-r12, r14}        
                            
#- Disable Interrupt and switch back in IRQ mode
#        mrs	r12, CPSR
#        bic     r12, r12, #ARM_MODE_SYS
#        orr     r12, r12, #I_BIT|ARM_MODE_IRQ
#        msr     CPSR_c, r12        
        
        ldr r0,=OSIntCtxSwFlag
        ldr r1,[r0]
        cmp r1,#1
        beq _IntCtxSw 
        
#- Mark the End of Interrupt on the AIC
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]         

		ldmfd sp!,{r0-r12,lr}
        subs pc,lr,#4     

_IntCtxSw:#真正的切换程序
        mov r1,#0
        str r1,[r0]#清除flag

        ldmfd sp!,{r0-r12,lr}#堆栈在内核外 内核只有堆栈指针 这里回复旧任务的现场 
        stmfd sp!,{r0-r3}   #后面会用到所以先压了   
        mov r1,sp                  //r1=sp   中断模式的sp 赋给r1
        add sp,sp,#16            //sp+4
        sub r2,lr,#4               //r2=old task pc 真正的lr的pc （由于流水线结构所以是4）

        mrs r3,spsr              //spsr=r0=old cpsr intclose   r3=old cpsr    关中断  spsr是上个任务的cpsr也就是a任务的cpsr
        orr r0,r3,#NOINT
        msr spsr_c,r0
        
        mrs     r0, CPSR       //切换到主程序模式    手动异常恢复 之前用的sp 是中断的sp
        orr     r0, r0, #ARM_MODE_SYS
        msr     CPSR_c, r0
        
        #ldr r0,=.+8
        #movs pc,r0

        stmfd sp!,{r2}              @ push old task's pc//此时的sp是系统模式的sp                                             任务切换自我 讲解
        stmfd sp!,{r4-r12,lr}       @ push old task's lr,r12-r4
        mov r4,r1                   @ Special optimised code below
        mov r5,r3
        ldmfd r4!,{r0-r3}#恢复原来任务的r0 - r3
        stmfd sp!,{r0-r3}           @ push old task's r3-r0
        stmfd sp!,{r5}              @ push old task's psr
        mrs r4,spsr
        stmfd sp!,{r4}              @ push old task's spsr
        
        @ OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy
        ldrb r5,[r5]
        strb r5,[r4]
        
        @ Get current task TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]
        str sp,[r5]                 @ store sp in preempted tasks's TCB

        bl OSTaskSwHook             @ call Task Switch Hook

        @ Get highest priority task TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr sp,[r6]                 @ get new task's stack pointer

        @ OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 @ set new current task TCB address

        ldmfd sp!,{r4}              @ pop new task's spsr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r4}              @ pop new task's psr
        msr CPSR_cxsf,r4        
 
#- Mark the End of Interrupt on the AIC 清除 中断 interrupt controlner
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]     
        
        
        ldmfd sp!,{r0-r12,lr,pc}    @ pop new task's r0-r12,lr & pc

              ##########总结： 中断与中断的切换关键就在于 旧任务现场的是从哪里来的
#*********************************************************************************************************
#                                   CRITICAL SECTION METHOD 3 FUNCTIONS
#
# Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
#              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
#              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
#              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
#              into the CPU's status register.
#
#              OS_CPU_SR OSCPUSaveSR()
# Arguments  : none
#
# Returns    : OS_CPU_SR
#
#              OSCPURestoreSR(OS_CPU_SR cpu_sr)
# Arguments  : OS_CPU_SR
#
# Returns    : none
#
# Note(s)    : These functions are used in general like this,
#
#            void Task (void *data)
#            {
#                    #if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
#                          OS_CPU_SR  cpu_sr#
#                    #endif
#                         :
#                         :
#                    OS_ENTER_CRITICAL()# /* cpu_sr = OSCPUSaveSR()# */
#                         :
#                         :
#                    OS_EXIT_CRITICAL()#  /* OSCPURestoreSR(cpu_sr)# */
#                         :
#                         :
#            }
#*********************************************************************************************************

        .global  OSCPUSaveSR #.global 可以被外部调用
OSCPUSaveSR:

        mrs r0,CPSR #r0 是c的返回值 读返回值的时候会读r0
        orr r1,r0,#NOINT
        msr CPSR_c,r1
        mov pc,lr   #返回原地址        asm调用c的函数 r0 作为返回值 r0同时作为输入多了就加一个r1


        .global  OSCPURestoreSR
OSCPURestoreSR:

        msr CPSR_c,r0
        mov pc,lr


        .END
        