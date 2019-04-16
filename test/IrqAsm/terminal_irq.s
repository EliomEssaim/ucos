/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Terminal_irq.s
    作者:     章杰
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  章杰
       描述:  编码完成
    2. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能，给按键增加中断入口函数
---------------------------------------------------------------------------*/
#           .INCLUDE     "../periph/aic/irq.mac"
            .INCLUDE     "../include/At91M55800.inc"

#------------------------------------------------------------------------------
#- 功能:		内部串口0中断服务程序
#- 调用程序:		irq_TERMINAL0_handler
#- 调用宏:		IRQ_ENTRY, IRQ_EXIT
#- 编写时间:		2004年4月14日
#- 测试结果:		
#------------------------------------------------------------------------------
	.GLOBAL Usart0AsmIrqhandler
	.EXTERN Usart0CHandler
#--------------------------------------------------------------------------------------
Usart0AsmIrqhandler:
#- Adjust and save LR_irq in IRQ stack
        sub     r14, r14, #4
        stmfd   sp!, {r14}

#- Save SPSR and r0 in IRQ stack
        mrs     r14, SPSR
        stmfd   sp!, {r12, r14}

#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
        ldr     r14, =AIC_BASE
        str     r14, [r14, #AIC_IVR]
            
#- Enable Interrupt and Switch in SYS Mode
        mrs     r12, CPSR
#进入中断时不应该打开中断，防止tick中断打断，这样会无法清AIC
#造成下次串口中断无法进入
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#有操作系统时需要调用        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart0CHandler         
        mov     r14,pc                   
        bx      r12                     /* Branch to at91_terminal0_c_handler */      
#有操作系统时需要调用        
.IFDEF OS_START        
        ldr     r12, =OSIntExit          
        mov     r14,pc                   
        bx      r12                     /* Branch to OSIntExit 	*/
.ENDIF        
 
# Interrupt Exit if no higher priority task ready to run
         
# restore interrupted task  
#- Restore scratch/used registers and LR from System Stack
        ldmfd   sp!, { r0-r12, r14}
                    
#- Disable Interrupt and switch back in IRQ mode
        mrs	r12, CPSR
        bic     r12, r12, #ARM_MODE_SYS
        orr     r12, r12, #I_BIT|ARM_MODE_IRQ
        msr     CPSR_c, r12
        
#- Mark the End of Interrupt on the AIC
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]
#- Restore SPSR_irq and r0 from IRQ stack
        ldmfd   sp!, {r12, r14}
        msr     SPSR_c, r14
#- Restore adjusted  LR_irq from IRQ stack directly in the PC
        ldmfd   sp!, {pc}^
        #.END
#--------------------------------------------------------------------------------------


#------------------------------------------------------------------------------
#- 功能:		内部串口1中断服务程序
#- 调用程序:		irq_TERMINAL1_handler
#- 调用宏:		IRQ_ENTRY, IRQ_EXIT
#- 编写时间:		2004年4月14日
#- 测试结果:		
#------------------------------------------------------------------------------
	.GLOBAL Usart1AsmIrqhandler
	.EXTERN Usart1CHandler
#--------------------------------------------------------------------------------------
Usart1AsmIrqhandler:
#- Adjust and save LR_irq in IRQ stack
        sub     r14, r14, #4
        stmfd   sp!, {r14}

#- Save SPSR and r0 in IRQ stack
        mrs     r14, SPSR
        stmfd   sp!, {r12, r14}

#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
        ldr     r14, =AIC_BASE
        str     r14, [r14, #AIC_IVR]
    
#- Enable Interrupt and Switch in SYS Mode
        mrs     r12, CPSR
#进入中断时不应该打开中断，防止tick中断打断，这样会无法清AIC
#造成下次串口中断无法进入
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#有操作系统时需要调用        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart1CHandler         
        mov     r14,pc                   
        bx      r12
#有操作系统时需要调用        
.IFDEF OS_START                             /* Branch to at91_terminal0_c_handler */      
        ldr     r12, =OSIntExit          
        mov     r14,pc                   
        bx      r12                     /* Branch to OSIntExit 	*/
.ENDIF        
 
# Interrupt Exit if no higher priority task ready to run
         
# restore interrupted task  
#- Restore scratch/used registers and LR from System Stack
        ldmfd   sp!, { r0-r12, r14}
                    
#- Disable Interrupt and switch back in IRQ mode
        mrs	r12, CPSR
        bic     r12, r12, #ARM_MODE_SYS
        orr     r12, r12, #I_BIT|ARM_MODE_IRQ
        msr     CPSR_c, r12
        
#- Mark the End of Interrupt on the AIC
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]
#- Restore SPSR_irq and r0 from IRQ stack
        ldmfd   sp!, {r12, r14}
        msr     SPSR_c, r14
#- Restore adjusted  LR_irq from IRQ stack directly in the PC
        ldmfd   sp!, {pc}^
        #.END
#--------------------------------------------------------------------------------------


#------------------------------------------------------------------------------
#- 功能:		内部串口2中断服务程序
#- 调用程序:		irq_TERMINAL0_handler
#- 调用宏:		IRQ_ENTRY, IRQ_EXIT
#- 编写时间:		2004年4月14日
#- 测试结果:		
#------------------------------------------------------------------------------
	.GLOBAL Usart2AsmIrqhandler
	.EXTERN Usart0CHandler
#--------------------------------------------------------------------------------------
Usart2AsmIrqhandler:
#- Adjust and save LR_irq in IRQ stack
        sub     r14, r14, #4
        stmfd   sp!, {r14}

#- Save SPSR and r0 in IRQ stack
        mrs     r14, SPSR
        stmfd   sp!, {r12, r14}

#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
        ldr     r14, =AIC_BASE
        str     r14, [r14, #AIC_IVR]
    
#- Enable Interrupt and Switch in SYS Mode
        mrs     r12, CPSR
#进入中断时不应该打开中断，防止tick中断打断，这样会无法清AIC
#造成下次串口中断无法进入
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#有操作系统时需要调用        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart2CHandler        
        mov     r14,pc                   
        bx      r12
#有操作系统时需要调用        
.IFDEF OS_START                             /* Branch to at91_terminal0_c_handler */      
        ldr     r12, =OSIntExit          
        mov     r14,pc                   
        bx      r12                     /* Branch to OSIntExit 	*/
.ENDIF        
 
# Interrupt Exit if no higher priority task ready to run
         
# restore interrupted task  
#- Restore scratch/used registers and LR from System Stack
        ldmfd   sp!, { r0-r12, r14}
                    
#- Disable Interrupt and switch back in IRQ mode
        mrs	r12, CPSR
        bic     r12, r12, #ARM_MODE_SYS
        orr     r12, r12, #I_BIT|ARM_MODE_IRQ
        msr     CPSR_c, r12
        
#- Mark the End of Interrupt on the AIC
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]
#- Restore SPSR_irq and r0 from IRQ stack
        ldmfd   sp!, {r12, r14}
        msr     SPSR_c, r14
#- Restore adjusted  LR_irq from IRQ stack directly in the PC
        ldmfd   sp!, {pc}^
        #.END
#--------------------------------------------------------------------------------------

#MCM-20_20061107_Linwei_begin
#有操作系统时需要调用(键盘在BootLoader中不使用)
.IFDEF OS_START
#------------------------------------------------------------------------------
#- 功能:		IRQ1中断服务程序
#- 调用程序:		irq_TERMINAL0_handler
#- 调用宏:		IRQ_ENTRY, IRQ_EXIT
#- 编写时间:		2006年6月29日
#- 测试结果:		
#------------------------------------------------------------------------------
	.GLOBAL KeyScanHandler
	.EXTERN KeyScanInterrupt
#--------------------------------------------------------------------------------------
KeyScanHandler:
#- Adjust and save LR_irq in IRQ stack
        sub     r14, r14, #4
        stmfd   sp!, {r14}

#- Save SPSR and r0 in IRQ stack
        mrs     r14, SPSR
        stmfd   sp!, {r12, r14}

#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
        ldr     r14, =AIC_BASE
        str     r14, [r14, #AIC_IVR]
    
#- Enable Interrupt and Switch in SYS Mode
        mrs     r12, CPSR
#进入中断时不应该打开中断，防止tick中断打断，这样会无法清AIC
#造成下次串口中断无法进入
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
       
        ldr     r12, =KeyScanInterrupt        
        mov     r14,pc                   
        bx      r12
    
        ldr     r12, =OSIntExit          
        mov     r14,pc                   
        bx      r12                     /* Branch to OSIntExit 	*/
        
 
# Interrupt Exit if no higher priority task ready to run
         
# restore interrupted task  
#- Restore scratch/used registers and LR from System Stack
        ldmfd   sp!, { r0-r12, r14}
                    
#- Disable Interrupt and switch back in IRQ mode
        mrs	r12, CPSR
        bic     r12, r12, #ARM_MODE_SYS
        orr     r12, r12, #I_BIT|ARM_MODE_IRQ
        msr     CPSR_c, r12
        
#- Mark the End of Interrupt on the AIC
        ldr     r12, =AIC_BASE
        str	r12, [r12, #AIC_EOICR]
#- Restore SPSR_irq and r0 from IRQ stack
        ldmfd   sp!, {r12, r14}
        msr     SPSR_c, r14
#- Restore adjusted  LR_irq from IRQ stack directly in the PC
        ldmfd   sp!, {pc}^
        #.END
#--------------------------------------------------------------------------------------
.ENDIF
#MCM-20_20061107_Linwei_end