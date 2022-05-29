.text 

.global TIM2_IRQHandler
.global atomic_inc
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

.type atomic_inc, function
atomic_inc:
push {r1, r2}
mov r1, r0
ldrex r2, [r1]
add r2, #1
strex r0, r2, [r1]
pop {r1, r2}
bx lr

.end
