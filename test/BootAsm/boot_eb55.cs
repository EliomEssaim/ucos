
;OUR_BOARD

stop
reset
delay
stop
memwrite 	0xFFFF4020 0x002F0002	; Clock Generator MOSCEN = 1, OSCOUNT = 47
delay
memwrite 	0xFFFF4020 0x002F4002	; Clock Generator MOSCEN = 1, OSCOUNT = 47

memwrite 	0xFFFF4020 0x032F4102	; Clock Generator MUL = 1, PLLCOUNT = 3, CSS = 1
delay
memwrite 	0xFFFF4020 0x032F8102	; Clock Generator CSS = 2, MUL = 1
memwrite 	0xffe00000 0x01003529	; EBI_CSR0 Flash Base Address 0x1000000
memwrite 	0xffe00004 0x02003121	; EBI_CSR1 SRAM Base Address 0x2000000
memwrite 	0xffe00020 0x1			; Remap

;FT_BOARD
;reset
;stop
;memwrite 	0xFFFF4020 0x002F0002	; Clock Generator MOSCEN = 1, OSCOUNT = 47
;delay
;memwrite 	0xFFFF4020 0x002F4002	; Clock Generator MOSCEN = 1, OSCOUNT = 47
;memwrite 	0xFFFF4020 0x032F4002	; Clock Generator MUL = 1, PLLCOUNT = 3, CSS = 1
;delay
;memwrite 	0xFFFF4020 0x032F8002	; Clock Generator CSS = 2, MUL = 1
;memwrite 	0xffe00000 0x01003529	; EBI_CSR0 Flash Base Address 0x1000000
;memwrite 	0xffe00004 0x02003121	; EBI_CSR1 SRAM Base Address 0x2000000
;memwrite 	0xffe00020 0x1			; Remap

;EMBEST_BOARD
;reset
;stop
;memwrite 	0xFFFF4020 0x002F0002	; Clock Generator MOSCEN = 1, OSCOUNT = 47
;delay
;memwrite 	0xFFFF4020 0x002F4002	; Clock Generator MOSCEN = 1, OSCOUNT = 47
;memwrite 	0xFFFF4020 0x032F4102	; Clock Generator MUL = 1, PLLCOUNT = 3, CSS = 1
;delay
;memwrite 	0xFFFF4020 0x032F8102	; Clock Generator CSS = 2, MUL = 1
;memwrite 	0xffe00000 0x01002529	; EBI_CSR0 Flash Base Address 0x1000000
;memwrite 	0xffe00004 0x02002121	; EBI_CSR1 SRAM Base Address 0x2000000
;memwrite 	0xffe00020 0x1			; Remap
