
# Hardfault Recovery for Cortex M4 (Maybe also M3 and M33 but I need to check docs)

Example "startup.s" for TM4C123GH6PM 
- Assuming other fault managers not enabled (only hardfault enabled)
- Checks what caused the fault, if recoverable return to next instruction (PC++)


Will return to next instruction (if possible) upon a hardfault

Can also be modified to display stack parameters (R0-R12, LR, SP, PC) 

# Snippet of handler recovery
''' 

	HardFault_Handler\
		                PROC
		                EXPORT  HardFault_Handler         [WEAK]
		
					movw    r3, #0xed29    		; get SCB BFFSR
					mov32    r3, #0xe000     	; 0xE000ED20 for MMFSR
					ldrb    r0, [r3, #0]    	

				; Check if it is possible to recover
					tst    r0, #0x2         	; check if it is a Precise ERR (pc points to fault)
					beq     _check_instruction_error  
					ldr     r3, [sp, #0x20] 	; Get PC from stack
					ldrh    r0, [r3, #0]    	; increment PC 
					add     r3, r3, #2     
					and     r0, r0, #0xf800 
					cmp     r0, #0xf800     
					ITTT    ne              
					cmpne   r0, #0xf000     	; 
					cmpne   r0, #0xe800     
					bne     _get_return_address       
					adds    r3, #2          

	_get_return_address                      
					str r3, [sp, #0x20]    		; Put PC+2 back into stack 
					b   _end_hardfault            

	_check_instruction_error					; Causes: Branch to invalid mem, invalid ret due to corrupt stack, Incorrect entry in the exception vector table
					tst r0, #0x01           	; Check if it is a BUSERR, instruction bus error
					beq _end_hardfault            
					ldr    r3, [sp, #0x1c]  	; BFAR does not write fault register 
					str    r3, [sp, #0x20]   	; Must get PC from stack
					b _end_hardfault              

	_end_hardfault                    		  
					movw r3, #0xed29    		; clear flag of fault    
					movt r3, #0xe000        
					movw r0, #0x3           
					strb r3, [r0, #0]
					BX LR
	;                B       .
	                ENDP
	
