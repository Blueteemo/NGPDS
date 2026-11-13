#ifdef __arm__

#include "K2Audio/SN76496.i"
#include "AudioMode.h"

	.extern pauseEmulation

	.global k2Audio_0

	.global soundInit
	.global soundReset
	.global soundRender
	.global soundSetMuteGUI
	.global setMuteT6W28
	.global soundUpdate
	.global T6W28_R_W
	.global T6W28_L_W
	.global T6W28_DAC_L_W
	.global T6W28_DAC_R_W

#define WAV_BUFFER_SIZE (0x800)
#define SHIFTVAL (21)

;@----------------------------------------------------------------------------

	.syntax unified
	.arm

#ifdef NDS
	;@ Scanline and PSG register hooks run on the ARM9's hot path.
	.section .itcm, "ax", %progbits
#else
	.section .text
#endif
	.align 2
;@----------------------------------------------------------------------------
soundInit:
	.type soundInit STT_FUNC
;@----------------------------------------------------------------------------
//	stmfd sp!,{lr}

//	ldmfd sp!,{lr}
//	bx lr

;@----------------------------------------------------------------------------
soundReset:
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}
	mov r0,#0
	ldr r1,=k2Audio_0
	bl sn76496Reset			;@ sound
	ldmfd sp!,{lr}
	b soundResetAudioBuffers

;@----------------------------------------------------------------------------
soundSetMuteGUI:
	.type   soundSetMuteGUI STT_FUNC
;@----------------------------------------------------------------------------
	ldr r1,=pauseEmulation		;@ Output silence when emulation paused.
	ldrb r0,[r1]
	strb r0,muteSoundGUI
	bx lr
;@----------------------------------------------------------------------------
setMuteT6W28:
;@----------------------------------------------------------------------------
	and r0,r0,#0xFF
	cmp r0,#0xAA
	cmpne r0,#0x55
	andeq r0,r0,#0xAA
	bxne lr
	strb r0,muteSoundChip
	bx lr
;@----------------------------------------------------------------------------
soundRender:					;@ r0=length, r1=pointer
;@----------------------------------------------------------------------------
;@	mov r11,r11
	ldr r2,muteSound
	tst r2,#0x00FF
	bne silenceMix
	tst r2,#0xFF00
	bne playSamples

	ldr r2,=soundMode
	ldrb r2,[r2]
	cmp r2,#AUDIO_STANDARD
	beq mixEnhanced
	stmfd sp!,{r0,lr}
	ldr r2,=k2Audio_0
	bl sn76496MixerLegacy
	ldmfd sp!,{r0,pc}

mixEnhanced:
	;@ Use the enhanced mixer with the same stream callback as legacy audio.
	stmfd sp!,{r0,lr}
	ldr r2,=k2Audio_0
	bl sn76496Mixer
	ldmfd sp!,{r0,pc}

	.macro writeDacFrame
	str r3,[r1],#4
	str r3,[r1],#4
	str r3,[r1],#4
	str r3,[r1],#4
	add r12,r12,#1<<SHIFTVAL
	subs r0,r0,#4
	.endm

playSamples:
	stmfd sp!,{r0,r1,lr}
	ldr r12,pcmWritePtr
	sub r12,r12,r0,lsr#2
	ldr r2,=WAVBUFFER
	mov r12,r12,lsl#SHIFTVAL
	ldr lr,=soundMode
	ldrb lr,[lr]
	cmp lr,#AUDIO_LEGACY
	bne stereoWavLoop

monoWavLoop:
	ldrb r3,[r2,r12,lsr#SHIFTVAL-1]
	eor r3,r3,#0x80
	mov r3,r3,lsl#8
	orr r3,r3,r3,lsl#16
	writeDacFrame
	bhi monoWavLoop
	ldmfd sp!,{r0,r1,lr}
	bx lr

stereoWavLoop:
	mov r3,r12,lsr#SHIFTVAL-1
	ldrh r3,[r2,r3]			;@ A2 left, A3 right.
	eor r3,r3,#0x80
	eor r3,r3,#0x8000
	and lr,r3,#0xff00
	and r3,r3,#0xff
	mov r3,r3,lsl#8
	orr r3,r3,lr,lsl#16
	writeDacFrame
	bhi stereoWavLoop
	ldmfd sp!,{r0,r1,lr}
	b soundFilterDac

silenceMix:
	mov r12,r0
	ldr r2,=soundMode
	ldrb r2,[r2]
	cmp r2,#AUDIO_LEGACY
	ldreq r2,=0x80008000
	movne r2,#0
silenceLoop:
	subs r12,r12,#1
	strpl r2,[r1],#4
	bhi silenceLoop

	bx lr

;@----------------------------------------------------------------------------
soundUpdate:
;@----------------------------------------------------------------------------
	ldr r0,muteSound
	tst r0,#0xff00
	bne updateDac
	bx lr

updateDac:
	ldr r2,pcmWritePtr
	add r1,r2,#1
	str r1,pcmWritePtr
	ldr r1,=WAVBUFFER
	mov r2,r2,lsl#SHIFTVAL		;@ Only keep 11 bits
	add r1,r1,r2,lsr#SHIFTVAL-1
	ldrh r0,dacLeft
	strh r0,[r1]
	bx lr
;@----------------------------------------------------------------------------
T6W28_DAC_L_W:
;@----------------------------------------------------------------------------
	strb r0,dacLeft
	bx lr
;@----------------------------------------------------------------------------
T6W28_DAC_R_W:
;@----------------------------------------------------------------------------
	strb r0,dacRight
	bx lr
;@----------------------------------------------------------------------------
T6W28_R_W:				;@ Sound right write
;@----------------------------------------------------------------------------
	ldr r1,=k2Audio_0
	b sn76496W
;@----------------------------------------------------------------------------
T6W28_L_W:				;@ Sound left write
;@----------------------------------------------------------------------------
	ldr r1,=k2Audio_0
	b sn76496LW

;@----------------------------------------------------------------------------
pcmWritePtr:	.long 0
pcmReadPtr:		.long 0
dacLeft:		.byte 0
dacRight:		.byte 0
	.space 2

muteSound:
muteSoundGUI:
	.byte 0xFF
muteSoundChip:
	.byte 0
	.space 2

#ifdef NDS
	.section .sbss				;@ Keep the PSG state in ARM9 DTCM.
#elif defined(GBA)
	.section .sbss				;@ This is EWRAM on GBA with devkitARM
#else
	.section .bss
#endif
	.align 2
k2Audio_0:
	.space snSize
#ifdef NDS
	.section .bss
	.align 2
#endif
WAVBUFFER:
	.space WAV_BUFFER_SIZE*2
;@----------------------------------------------------------------------------
	.end
#endif // __arm__
