.text 

.global PendSV_Handler
.global SysTick_Handler
.global TIM2_IRQHandler
.global TIM3_IRQHandler
.global USART1_IRQHandler
.global atomic_inc
.global rcvr_spi_multi
.code 16
.syntax unified

.type PendSV_Handler, function
PendSV_Handler:
push {r4-r11, lr}
mov r0, sp
/*
tst lr, #0x4
ite eq
mrseq r0, msp
mrsne r0, psp
*/
bl PendSV_Handler_local
mov sp, r0
pop {r4-r11, pc}

.type SysTick_Handler, function
SysTick_Handler:
push {r4-r11, lr}
mov r0, sp
/*
tst lr, #0x4
ite eq
mrseq r0, msp
mrsne r0, psp
*/
bl SysTick_Handler_local
mov sp, r0
pop {r4-r11, pc}

.type TIM2_IRQHandler, function
TIM2_IRQHandler:
push {r4-r11, lr}
mov r0, sp
bl TIM2_IRQHandler_local
mov sp, r0
pop {r4-r11, pc}

.type TIM3_IRQHandler, function
TIM3_IRQHandler:
push {r4-r11, lr}
mov r0, sp
bl TIM3_IRQHandler_local
mov sp, r0
pop {r4-r11, pc}

.type USART1_IRQHandler, function
USART1_IRQHandler:
push {r4-r11, lr}
mov r0, sp
bl USART1_IRQHandler_local
mov sp, r0
pop {r4-r11, pc}

.type atomic_inc, function
atomic_inc:
push {r1, r2}
mov r1, r0
ldrex r2, [r1]
add r2, #1
strex r0, r2, [r1]
pop {r1, r2}
bx lr

.type rcvr_spi_multi, function
rcvr_spi_multi:
push {r2-r3}

/*r0=char*buff, r1=lens*/
/*r3=base of spi, r2=tmp data*/
ldr r3, =0x40013000

/*while((SD_SPI->SR & SPI_I2S_FLAG_TXE) == RESET);*/
wait_tx_done:
nop
ldrh r2, [r3, #8]
uxth r2, r2
and.w r2, r2, #2
cmp r2, #0
beq.n wait_tx_done

/*SD_SPI->DR = 0xff;*/
movs r2, #255
strh r2, [r3, #12]

/*while((SD_SPI->SR & SPI_I2S_FLAG_RXNE) == RESET);*/
wait_rx_done:
nop
ldrh r2, [r3, #8]
uxth r2, r2
and.w r2, r2, #1
cmp r2, #0
beq.n wait_rx_done

/* *(buff+i) = SD_SPI->DR; */
ldrh r2, [r3, #12]
uxth r2, r2
strb r2, [r0, #0]
adds r0, r0, #1
subs r1, r1, #1
cmp r1, #0
bne.n wait_tx_done

pop {r2-r3}
bx lr

.end
