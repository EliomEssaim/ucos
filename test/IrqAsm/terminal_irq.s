/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Terminal_irq.s
    ����:     �½�
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/09/28
       ����:  �½�
       ����:  �������
    2. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ����ܣ������������ж���ں���
---------------------------------------------------------------------------*/
#           .INCLUDE     "../periph/aic/irq.mac"
            .INCLUDE     "../include/At91M55800.inc"

#------------------------------------------------------------------------------
#- ����:		�ڲ�����0�жϷ������
#- ���ó���:		irq_TERMINAL0_handler
#- ���ú�:		IRQ_ENTRY, IRQ_EXIT
#- ��дʱ��:		2004��4��14��
#- ���Խ��:		
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
#�����ж�ʱ��Ӧ�ô��жϣ���ֹtick�жϴ�ϣ��������޷���AIC
#����´δ����ж��޷�����
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#�в���ϵͳʱ��Ҫ����        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart0CHandler         
        mov     r14,pc                   
        bx      r12                     /* Branch to at91_terminal0_c_handler */      
#�в���ϵͳʱ��Ҫ����        
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
#- ����:		�ڲ�����1�жϷ������
#- ���ó���:		irq_TERMINAL1_handler
#- ���ú�:		IRQ_ENTRY, IRQ_EXIT
#- ��дʱ��:		2004��4��14��
#- ���Խ��:		
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
#�����ж�ʱ��Ӧ�ô��жϣ���ֹtick�жϴ�ϣ��������޷���AIC
#����´δ����ж��޷�����
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#�в���ϵͳʱ��Ҫ����        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart1CHandler         
        mov     r14,pc                   
        bx      r12
#�в���ϵͳʱ��Ҫ����        
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
#- ����:		�ڲ�����2�жϷ������
#- ���ó���:		irq_TERMINAL0_handler
#- ���ú�:		IRQ_ENTRY, IRQ_EXIT
#- ��дʱ��:		2004��4��14��
#- ���Խ��:		
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
#�����ж�ʱ��Ӧ�ô��жϣ���ֹtick�жϴ�ϣ��������޷���AIC
#����´δ����ж��޷�����
        #bic     r12, r12, #I_BIT
        orr     r12, r12, #ARM_MODE_SYS
        msr     CPSR_c, r12              

#- Save scratch/used registers and LR in SYS Stack
        stmfd   sp!, { r0-r12,r14}
#�в���ϵͳʱ��Ҫ����        
.IFDEF OS_START	            	
        ldr     r12, =OSIntEnter         
        mov     r14,pc                   
        bx      r12			 			/* Branch to OsIntEnter */
.ENDIF        
        ldr     r12, =Usart2CHandler        
        mov     r14,pc                   
        bx      r12
#�в���ϵͳʱ��Ҫ����        
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
#�в���ϵͳʱ��Ҫ����(������BootLoader�в�ʹ��)
.IFDEF OS_START
#------------------------------------------------------------------------------
#- ����:		IRQ1�жϷ������
#- ���ó���:		irq_TERMINAL0_handler
#- ���ú�:		IRQ_ENTRY, IRQ_EXIT
#- ��дʱ��:		2006��6��29��
#- ���Խ��:		
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
#�����ж�ʱ��Ӧ�ô��жϣ���ֹtick�жϴ�ϣ��������޷���AIC
#����´δ����ж��޷�����
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