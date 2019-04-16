        .global  CPUSaveSR
CPUSaveSR:

        mrs r0,CPSR
        orr r1,r0,#0xC0
        msr CPSR_c,r1
        mov pc,lr


        .global  CPURestoreSR
CPURestoreSR:

        msr CPSR_c,r0
        mov pc,lr
        
        .global  JMPTORAM
JMPTORAM:

        mov r0,#0x2000000
        bx r0 
        
             


        