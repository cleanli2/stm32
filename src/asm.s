.text 

.global TIM2_IRQHandler
.code 16 
.syntax unified


.type TIM2_IRQHandler, function 
TIM2_IRQHandler:
push {r7, lr}
mov r0, sp
/*
tst lr, #0x4
ite eq
mrseq r0, msp
mrsne r0, psp
*/
bl TIM2_IRQHandler_local
mov sp, r0
pop {r7, pc}

.end
