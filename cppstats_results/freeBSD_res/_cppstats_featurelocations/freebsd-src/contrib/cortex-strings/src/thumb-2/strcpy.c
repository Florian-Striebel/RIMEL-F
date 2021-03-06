


































#if defined(__thumb2__)
#define magic1(REG) "#0x01010101"
#define magic2(REG) "#0x80808080"
#else
#define magic1(REG) #REG
#define magic2(REG) #REG ", lsl #7"
#endif

char* __attribute__((naked))
strcpy (char* dst, const char* src)
{
asm (
#if !(defined(__OPTIMIZE_SIZE__) || defined (PREFER_SIZE_OVER_SPEED) || (defined (__thumb__) && !defined (__thumb2__)))

"pld [r1, #0]\n\t"
"eor r2, r0, r1\n\t"
"mov ip, r0\n\t"
"tst r2, #3\n\t"
"bne 4f\n\t"
"tst r1, #3\n\t"
"bne 3f\n"
"5:\n\t"
#if !defined(__thumb2__)
"str r5, [sp, #-4]!\n\t"
"mov r5, #0x01\n\t"
"orr r5, r5, r5, lsl #8\n\t"
"orr r5, r5, r5, lsl #16\n\t"
#endif

"str r4, [sp, #-4]!\n\t"
"tst r1, #4\n\t"
"ldr r3, [r1], #4\n\t"
"beq 2f\n\t"
"sub r2, r3, "magic1(r5)"\n\t"
"bics r2, r2, r3\n\t"
"tst r2, "magic2(r5)"\n\t"
"itt eq\n\t"
"streq r3, [ip], #4\n\t"
"ldreq r3, [r1], #4\n"
"bne 1f\n\t"



".p2align 2\n"
"2:\n\t"
"pld [r1, #8]\n\t"
"ldr r4, [r1], #4\n\t"
"sub r2, r3, "magic1(r5)"\n\t"
"bics r2, r2, r3\n\t"
"tst r2, "magic2(r5)"\n\t"
"sub r2, r4, "magic1(r5)"\n\t"
"bne 1f\n\t"
"str r3, [ip], #4\n\t"
"bics r2, r2, r4\n\t"
"tst r2, "magic2(r5)"\n\t"
"itt eq\n\t"
"ldreq r3, [r1], #4\n\t"
"streq r4, [ip], #4\n\t"
"beq 2b\n\t"
"mov r3, r4\n"
"1:\n\t"
#if defined(__ARMEB__)
"rors r3, r3, #24\n\t"
#endif
"strb r3, [ip], #1\n\t"
"tst r3, #0xff\n\t"
#if defined(__ARMEL__)
"ror r3, r3, #8\n\t"
#endif
"bne 1b\n\t"
"ldr r4, [sp], #4\n\t"
#if !defined(__thumb2__)
"ldr r5, [sp], #4\n\t"
#endif
"BX LR\n"



"3:\n\t"
"tst r1, #1\n\t"
"beq 1f\n\t"
"ldrb r2, [r1], #1\n\t"
"strb r2, [ip], #1\n\t"
"cmp r2, #0\n\t"
"it eq\n"
"BXEQ LR\n"
"1:\n\t"
"tst r1, #2\n\t"
"beq 5b\n\t"
"ldrh r2, [r1], #2\n\t"
#if defined(__ARMEB__)
"tst r2, #0xff00\n\t"
"iteet ne\n\t"
"strneh r2, [ip], #2\n\t"
"lsreq r2, r2, #8\n\t"
"streqb r2, [ip]\n\t"
"tstne r2, #0xff\n\t"
#else
"tst r2, #0xff\n\t"
"itet ne\n\t"
"strneh r2, [ip], #2\n\t"
"streqb r2, [ip]\n\t"
"tstne r2, #0xff00\n\t"
#endif
"bne 5b\n\t"
"BX LR\n"



"4:\n\t"
"ldrb r2, [r1], #1\n\t"
"strb r2, [ip], #1\n\t"
"cmp r2, #0\n\t"
"bne 4b\n\t"
"BX LR"

#elif !defined (__thumb__) || defined (__thumb2__)
"mov r3, r0\n\t"
"1:\n\t"
"ldrb r2, [r1], #1\n\t"
"strb r2, [r3], #1\n\t"
"cmp r2, #0\n\t"
"bne 1b\n\t"
"BX LR"
#else
"mov r3, r0\n\t"
"1:\n\t"
"ldrb r2, [r1]\n\t"
"add r1, r1, #1\n\t"
"strb r2, [r3]\n\t"
"add r3, r3, #1\n\t"
"cmp r2, #0\n\t"
"bne 1b\n\t"
"BX LR"
#endif
);
}

