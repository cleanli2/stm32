.text 

.global PendSV_Handler
.global SysTick_Handler
.global TIM2_IRQHandler
.global TIM3_IRQHandler
.global USART1_IRQHandler
.global atomic_inc
.global rcvr_spi_multi
.global xmit_spi_multi
.global lcd_w16
.global rgb565_to_lcd
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

.type xmit_spi_multi, function
xmit_spi_multi:
push {r2-r3}

/*r0=char*buff, r1=lens*/
/*r3=base of spi, r2=tmp data*/
ldr r3, =0x40013000

/*while((SD_SPI->SR & SPI_I2S_FLAG_TXE) == RESET);*/
xwait_tx_done:
nop
ldrh r2, [r3, #8]
uxth r2, r2
and.w r2, r2, #2
cmp r2, #0
beq.n xwait_tx_done

/*SD_SPI->DR = *buff++;*/
ldrb r2, [r0, #0]
uxth r2, r2
strh r2, [r3, #12]
adds r0, r0, #1

/*len--*/
subs r1, r1, #1
cmp r1, #0
bne.n xwait_tx_done

pop {r2-r3}
bx lr


/*void lcd_w16(u16 color)*/
.type lcd_w16, function
lcd_w16:
push {r2, r3, r4}

/*r0=color, r1 no use*/
/*r3, r2=tmp data*/

/*
   LCD_RS_SET;
   LCD_CS_CLR;
   DATAOUT(d);
   LCD_WR_CLR;
   LCD_WR_SET;
   LCD_CS_SET;
*/
ldr r2, =0x40010800
movs r3, #2
str r3, [r2, #16]
movs r3, #1
str r3, [r2, #20]
ldr r4, =0x40010c00
str r0, [r4, #12]
ldr r4, =0x40011000
mov.w r3, 0x8000
str r3, [r4, #20]
str r3, [r4, #16]
movs r3, #1
str r3, [r2, #16]

pop {r2, r3, r4}
bx lr

.type rgb565_to_lcd, function
rgb565_to_lcd:
push {r2-r3, lr}

/*r0->r3=char*buff, r1=lens*/
/*r3=buff, r2=tmp data*/

mov r3, r0

compute_color:
/*color=(bf[i]<<8)|bf[i+1];*/
ldrb r0, [r3, #0]
lsls r0, r0, #8
ldrb r2, [r3, #1]
orrs r0, r2
bl lcd_w16
adds r3, r3, #2

/*len--*/
subs r1, r1, #2
cmp r1, #1
beq.n endofrgb2lcd
cmp r1, #0
bne.n compute_color

endofrgb2lcd:
pop {r2-r3, pc}

.end
