#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#------------------------------------------------------------------------------
#- File source          : irq_end.s
#- Object               : Assembler Handler Exit Standard Sequence.
#-
#- 1.0 01/04/00 JCZ     : Creation
#- 1.1 02/11/00 JPP     : Green Hills
#------------------------------------------------------------------------------

#            AREA        reset, CODE, READONLY, INTERWORK

            .INCLUDE     "../irqasm/irq.mac"

#------------------------------------------------------------------------------
#- Function             : irq_end
#- Treatments           : Interrupt Handler Exit Sequence
#- Input Parameters     : None
#- Output Parameters    : None
#- Registers lost       : None
#- Called Functions     : None
#- Called Macros        : IRQ_EXIT
#------------------------------------------------------------------------------
            .global      irq_end
irq_end:
            IRQ_EXIT

#            END
