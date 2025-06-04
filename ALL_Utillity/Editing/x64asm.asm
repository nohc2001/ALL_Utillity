

.const
	float_4 REAL4 4.0f
	uint3 QWORD 3
	sin01_n3 REAL4 0.16666666f
	sin01_n5 REAL4 0.008f
	sin01_n7 REAL4 0.00019047619f
	sin01_n9 REAL4 0.0000027557f
	sin01_2pi REAL4 6.28318530718f
	float2 REAL4 2.0f
	enableFlags32 DWORD 4294967295
.data
	sin01_A_array REAL4 0.25f, -0.25f, 0.25f, -0.25f
	sin01_B_array REAL4 0.0f, 0.5f, -0.5f, 1.0f
	sin01_R_array REAL4 1.0f, 1.0f, -1.0f, -1.0f

DATA_SEG SEGMENT ALIGN(32) 'DATA'
	sin01_8_A_array REAL4 0.25f, -0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 0.0f
	sin01_8_B_array REAL4 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
	sin01_8_R_array REAL4 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f
    testarray real4 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.0  ; 8 floats, each initialized to 1.0
	sin01_8_float_4 REAL4 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0
	sin01_8_uint3 DWORD 3, 3, 3, 3, 3, 3, 3, 3
	sin01_8_n3 REAL4 0.16666666f, 0.16666666f, 0.16666666f, 0.16666666f, 0.16666666f, 0.16666666f, 0.16666666f, 0.16666666f
	sin01_8_n5 REAL4 0.008f, 0.008f, 0.008f, 0.008f, 0.008f, 0.008f, 0.008f, 0.008f
	sin01_8_n7 REAL4 0.00019047619f, 0.00019047619f, 0.00019047619f, 0.00019047619f, 0.00019047619f, 0.00019047619f, 0.00019047619f, 0.00019047619f
	sin01_8_n9 REAL4 0.0000027557f, 0.0000027557f, 0.0000027557f, 0.0000027557f, 0.0000027557f, 0.0000027557f, 0.0000027557f, 0.0000027557f
	sin01_8_2pi REAL4 6.28318530718f, 6.28318530718f, 6.28318530718f, 6.28318530718f, 6.28318530718f, 6.28318530718f, 6.28318530718f, 6.28318530718f
	sin01_enableFlags32 DWORD 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 ,4294967295, 4294967295, 4294967295
DATA_SEG ENDS

.code

; 프로시저의 첫번째 파라미터는 항상 c로 받는다.
; 아 이제보니 masm은 진짜 asm이 아니라 그냥 뭔가 수두코드같은거라 지원안되는 연산이 있구나 깨달음.

set_ret PROC
	mov [rsp], rcx
	ret
set_ret ENDP

p_ui32_rev PROC
	mov         eax,ecx  
	shr         ecx,1  
	and         eax,0D5555555h  
	and         ecx,55555555h  
	add         eax,eax  
	or          ecx,eax  
	mov         eax,ecx  
	shr         ecx,2  
	and         ecx,33333333h  
	and         eax,0F3333333h  
	shl         eax,2  
	or          eax,ecx  
	mov         ecx,eax  
	shr         eax,4  
	and         ecx,0FF0F0F0Fh  
	and         eax,0F0F0F0Fh  
	shl         ecx,4  
	or          ecx,eax  
	bswap       ecx
	mov			eax, ecx
	ret
p_ui32_rev ENDP

; subss xmm0, xmm1 : float subtraction xmm0 -= xmm1;
subf PROC
	subss xmm0, xmm1;
	ret 0
subf ENDP

sin01 PROC
	; param input in xmm0

	; 1. xmm0 -= (int)xmm0
	cvttss2si eax, xmm0	; eax = (int)xmm0;
    cvtsi2ss xmm1, eax	; xmm1 = (float)eax;
	subss xmm0, xmm1 ; xmm0 -= xmm1;

	; 2. xmm0 *= 4.0f;
	mulss xmm0, float_4; 

	; 3. eax = (int)xmm0;
	cvttss2si eax, xmm0	; eax = (int)xmm0;

	; 4. eax &= 3;
	and rax, uint3 ; rax &= 3;

	; 5. xmm0 *= A[eax];
	mov rbx, OFFSET sin01_A_array
	movss xmm1, dword ptr [rbx+rax*4]
	mulss xmm0, xmm1

	; 6. xmm0 += B[eax];
	mov rbx, OFFSET sin01_B_array
	movss xmm1, dword ptr [rbx+rax*4]
	addss xmm0, xmm1

	; 7. xmm0 *= sin01_2pi
	mulss xmm0, sin01_2pi

	; 8. xmm1 = xmm0;
	movss xmm1, xmm0

	; 9. xmm2 = xmm0 * xmm0;
	movss xmm2, xmm0
	mulss xmm2, xmm0

	; 10. xmm4 = xmm2 * xmm2;
	; 11. xmm3 = xmm2 * xmm0;
	movss xmm3, xmm2
	movss xmm4, xmm2
	mulss xmm3, xmm0
	mulss xmm4, xmm4

	; 12. xmm5 = xmm4 * xmm0
	; 13. xmm6 = xmm4 * xmm3
	; 14. xmm1 -= xmm3 * sin01_n3;
	movss xmm5, xmm0
	movss xmm6, xmm3
	mulss xmm5, xmm4
	mulss xmm6, xmm4

	mulss xmm3, sin01_n3
	subss xmm1, xmm3

	; 15. xmm4 *= xmm5
	; 16. xmm5 = xmm5 * sin01_n5;
	; 17. xmm6 = xmm6 * sin01_n7;
	mulss xmm4, xmm5
	mulss xmm5, sin01_n5
	mulss xmm6, sin01_n7

	; 18. xmm1 += xmm5;
	; 19. xmm4 *= sin01_n9
	addss xmm1, xmm5
	mulss xmm4, sin01_n9

	; 20. xmm1 -= xmm6
	subss xmm1, xmm6

	; 21. xmm1 += xmm4
	addss xmm1, xmm4

	; 22. xmm0 = sin01_R_array[eax] * xmm1; ret 0
	mov rbx, OFFSET sin01_R_array;
	movss xmm0, dword ptr [rbx+rax*4]
	mulss xmm0, xmm1

	ret 0
sin01 ENDP

sin01_ox PROC
  cvttss2si   eax,xmm0  
  cvtsi2ss    xmm1,eax  
  subss       xmm0,xmm1  
  mulss       xmm0,float_4  
  cvttss2si   eax,xmm0  
  and         rax,uint3  
  mov         rbx,OFFSET sin01_A_array  
  movss       xmm1,dword ptr [rbx+rax*4]  
  mulss       xmm0,xmm1  
  mov         rbx,OFFSET sin01_B_array  
  movss       xmm1,dword ptr [rbx+rax*4]  
  addss       xmm0,xmm1  
  mulss       xmm0,sin01_2pi
  movss       xmm1,xmm0  
  movss       xmm2,xmm0  
  mulss       xmm2,xmm0  
  movss       xmm3,xmm2  
  movss       xmm4,xmm2  
  mulss       xmm3,xmm0  
  mulss       xmm4,xmm4  
  movss       xmm5,xmm0  
  movss       xmm6,xmm3
  mulss       xmm5,xmm4
  mulss       xmm6,xmm4
  mulss       xmm3,sin01_n3
  subss       xmm1,xmm3
  mulss       xmm4,xmm5
  mulss       xmm5,sin01_n5
  mulss       xmm6,sin01_n7
  addss       xmm1,xmm5
  mulss       xmm4,sin01_n9
  subss       xmm1,xmm6
  addss       xmm1,xmm4
  mov         rbx,OFFSET sin01_R_array
  movss       xmm0,dword ptr [rbx+rax*4]
  mulss       xmm0,xmm1
  ret 0
sin01_ox ENDP


fa$ = 8
f1$ = 16
f2$ = 24
f3$ = 32
f4$ = 40
sin01_4 PROC
  cvttss2si   eax,xmm0  
  cvttss2si   r8,xmm1  
  cvtsi2ss    xmm4,eax
  cvtsi2ss    xmm9,r8  
  subss       xmm0,xmm4
  subss       xmm1,xmm9 
  mulss       xmm0,float_4  
  mulss       xmm1,float_4
  cvttss2si   eax,xmm0  
  cvttss2si   r8,xmm1 
  and         rax,uint3 
  and         r8,uint3  
  mov         rbx,OFFSET sin01_A_array  
  mov         r9,OFFSET sin01_A_array
  movss       xmm4,dword ptr [rbx+rax*4]  
  movss       xmm9,dword ptr [r9+r8*4]  
  mulss       xmm0,xmm4  
  mulss       xmm1,xmm9  
  mov         rbx,OFFSET sin01_B_array  
  mov         r9,OFFSET sin01_B_array 
  movss       xmm4,dword ptr [rbx+rax*4]  
  movss       xmm9,dword ptr [r9+r8*4]  
  addss       xmm0,xmm4 
  addss       xmm1,xmm9  
  mulss       xmm0,sin01_2pi
  mulss       xmm1,sin01_2pi
  movss       xmm4,xmm0  
  movss       xmm9,xmm1  
  ;
  movss       xmm5,xmm0  
  movss       xmm10,xmm1  
  mulss       xmm5,xmm0 
  mulss       xmm10,xmm1    
  movss       xmm6,xmm5
  movss       xmm11,xmm10  
  mulss       xmm5,xmm0
  mulss       xmm10,xmm1  
  mulss       xmm6,xmm6
  mulss       xmm11,xmm11  
  movss       xmm7,xmm0
  movss       xmm12,xmm1 
  movss       xmm8,xmm5
  movss       xmm13,xmm10
  cvttss2si   r10,xmm2  
  mulss       xmm7,xmm6
  cvttss2si   r12,xmm3  
  mulss       xmm12,xmm11
  cvtsi2ss    xmm14,r10
  mulss       xmm8,xmm6
  cvtsi2ss    xmm15,r12  
  mulss       xmm13,xmm11
  subss       xmm2,xmm14
  mulss       xmm5,sin01_n3
  subss       xmm3,xmm15 
  mulss       xmm10,sin01_n3
  mulss       xmm2,float_4  
  subss       xmm4,xmm5
  mulss       xmm3,float_4
  subss       xmm9,xmm10
  cvttss2si   r10,xmm2  
  mulss       xmm6,xmm7
  cvttss2si   r12,xmm3 
  mulss       xmm11,xmm12
  and         r10,uint3 
  mulss       xmm7,sin01_n5
  and         r12,uint3  
  mulss       xmm12,sin01_n5
  mov         r11,OFFSET sin01_A_array  
  mulss       xmm8,sin01_n7
  mov         r13,OFFSET sin01_A_array
  mulss       xmm13,sin01_n7
  movss       xmm14,dword ptr [r11+r10*4]  
  addss       xmm4,xmm7
  movss       xmm15,dword ptr [r13+r12*4]  
  addss       xmm9,xmm12
  mulss       xmm2,xmm14  
  mulss       xmm6,sin01_n9
  mulss       xmm3,xmm15  
  mulss       xmm11,sin01_n9
  mov         r11,OFFSET sin01_B_array  
  subss       xmm4,xmm8
  mov         r13,OFFSET sin01_B_array 
  subss       xmm9,xmm13
  movss       xmm14,dword ptr [r11+r10*4]  
  addss       xmm4,xmm6
  movss       xmm15,dword ptr [r13+r12*4]  
  addss       xmm9,xmm11
  addss       xmm2,xmm14 
  mov         rbx,OFFSET sin01_R_array
  addss       xmm3,xmm15  
  mov         r9,OFFSET sin01_R_array
  mulss       xmm2,sin01_2pi
  movss       xmm0,dword ptr [rbx+rax*4]
  mulss       xmm3,sin01_2pi
  movss       xmm1,dword ptr [r9+r8*4]
  movss       xmm14,xmm2
  mulss       xmm0,xmm4
  movss       xmm15,xmm3
  mulss       xmm1,xmm9
  ;
  movss       xmm5,xmm2  
  movss       xmm10,xmm3  
  mulss       xmm5,xmm2 
  mulss       xmm10,xmm3    
  movss       xmm6,xmm5
  movss       xmm11,xmm10  
  mulss       xmm5,xmm2
  mulss       xmm10,xmm3  
  mulss       xmm6,xmm6
  mulss       xmm11,xmm11  
  movss       xmm7,xmm2
  movss       xmm12,xmm3 
  movss       xmm8,xmm5
  movss       xmm13,xmm10
  mulss       xmm7,xmm6
  mulss       xmm12,xmm11
  mulss       xmm8,xmm6
  mulss       xmm13,xmm11
  mulss       xmm5,sin01_n3
  mulss       xmm10,sin01_n3
  subss       xmm14,xmm5
  subss       xmm15,xmm10
  mulss       xmm6,xmm7
  mulss       xmm11,xmm12
  mulss       xmm7,sin01_n5
  mulss       xmm12,sin01_n5
  mulss       xmm8,sin01_n7
  mulss       xmm13,sin01_n7
  addss       xmm14,xmm7
  addss       xmm15,xmm12
  mulss       xmm6,sin01_n9
  mulss       xmm11,sin01_n9
  subss       xmm14,xmm8
  subss       xmm15,xmm13
  addss       xmm14,xmm6
  addss       xmm15,xmm11
  mov         r11,OFFSET sin01_R_array
  mov         r13,OFFSET sin01_R_array
  movss       xmm2,dword ptr [r11+r10*4]
  movss       xmm3,dword ptr [r13+r12*4]
  mulss       xmm2,xmm14
  mulss       xmm3,xmm15
  ;
  movss   DWORD PTR [rcx], xmm1
  movss   DWORD PTR [rcx+4], xmm2
  movss   DWORD PTR [rcx+8], xmm3
  movss   DWORD PTR [rcx+12], xmm0
  ret     0
sin01_4 ENDP

sin01_4_sse PROC
	; param input in xmm0

	; 1. xmm0 -= (int)xmm0
	cvttss2si eax, xmm0	; eax = (int)xmm0;
    cvtsi2ss xmm1, eax	; xmm1 = (float)eax;
	subss xmm0, xmm1 ; xmm0 -= xmm1;

	; 2. xmm0 *= 4.0f;
	mulss xmm0, float_4; 

	; 3. eax = (int)xmm0;
	cvttss2si eax, xmm0	; eax = (int)xmm0;

	; 4. eax &= 3;
	and rax, uint3 ; rax &= 3;

	; 5. xmm0 *= A[eax];
	mov rbx, OFFSET sin01_A_array
	movss xmm1, dword ptr [rbx+rax*4]
	mulss xmm0, xmm1

	; 6. xmm0 += B[eax];
	mov rbx, OFFSET sin01_B_array
	movss xmm1, dword ptr [rbx+rax*4]
	addss xmm0, xmm1

	; 7. xmm0 *= sin01_2pi
	mulss xmm0, sin01_2pi

	; 8. xmm1 = xmm0;
	movss xmm1, xmm0

	; 9. xmm2 = xmm0 * xmm0;
	movss xmm2, xmm0
	mulss xmm2, xmm0

	; 10. xmm4 = xmm2 * xmm2;
	; 11. xmm3 = xmm2 * xmm0;
	movss xmm3, xmm2
	movss xmm4, xmm2
	mulss xmm3, xmm0
	mulss xmm4, xmm4

	; 12. xmm5 = xmm4 * xmm0
	; 13. xmm6 = xmm4 * xmm3
	; 14. xmm1 -= xmm3 * sin01_n3;
	movss xmm5, xmm0
	movss xmm6, xmm3
	mulss xmm5, xmm4
	mulss xmm6, xmm4

	mulss xmm3, sin01_n3
	subss xmm1, xmm3

	; 15. xmm4 *= xmm5
	; 16. xmm5 = xmm5 * sin01_n5;
	; 17. xmm6 = xmm6 * sin01_n7;
	mulss xmm4, xmm5
	mulss xmm5, sin01_n5
	mulss xmm6, sin01_n7

	; 18. xmm1 += xmm5;
	; 19. xmm4 *= sin01_n9
	addss xmm1, xmm5
	mulss xmm4, sin01_n9

	; 20. xmm1 -= xmm6
	subss xmm1, xmm6

	; 21. xmm1 += xmm4
	addss xmm1, xmm4

	; 22. xmm0 = sin01_R_array[eax] * xmm1; ret 0
	mov rbx, OFFSET sin01_R_array;
	movss xmm0, dword ptr [rbx+rax*4]
	mulss xmm0, xmm1

	ret 0
sin01_4_sse ENDP

;vroundps imm8 description
;vroundps	   ymm1,ymm0, 0 ; even
;vroundps	   ymm1,ymm0, 1 ; floor
;vroundps	   ymm1,ymm0, 2 ; ceil
;vroundps	   ymm1,ymm0, 3 ; truncate
;0b0000 (0): Round to nearest (even)
;0b0001 (1): Round towards negative infinity (floor)
;0b0010 (2): Round towards positive infinity (ceil)
;0b0011 (3): Round towards zero (truncate)
;0b0100 (4): Use current MXCSR rounding mode
;0b1000 (8): Suppress exceptions

;zen3 performance..
;nemonic latency infothrouput / expthroput portinfo
;vroundps 3	0.50 / 0.50	1	1*FP23
;vcvtps2dq 3 0.50 / 0.50 1 1*FP23
;vmovaps ymm, mem256 [≤8;≤9] 0.50 1 port??
;vmovaps mem256, ymm [≤9;≤10]	0.50 / 1.00	1	1*FP45
;vaddps, vsubps 3	0.50 / 0.50	1	1*FP23
;vmulps 3 0.50 / 0.50 1	1*FP01
;vpermps [3;8] 1.00 / 1.28 2 2*FP01

;vbroadcastss 1	0.50 / 1.00	1	1*FP01
;vpbroadcastd 1	0.50 / 1.00	1	1*FP01
;vgatherdps [≤0;≤28] 7.00 / 8.00 39	1*FP01+7*FP0123+7*FP1+9*FP45

sin01_8 PROC
;x -= (int)x;
  vmovaps      ymm0, [rcx]
  vroundps	   ymm1, ymm0, 1
  vsubps       ymm0, ymm0,ymm1
;x = 4.0f * x;
  vmovaps		ymm1, [sin01_8_float_4]
  vmulps       ymm0, ymm0, ymm1
;ui32 index = (int)x;
  vroundps	   ymm1, ymm0, 1
  vcvtps2dq	   ymm6,ymm1
;index &= 3;
  vmovaps		ymm1, [sin01_8_uint3]
  vpand         ymm6, ymm6, ymm1
;x = A[index] * x;
  vmovaps		ymm1, [sin01_8_A_array]
  vpermps		ymm1, ymm6, ymm1
  vmulps       ymm0, ymm0,ymm1
;x += B[index];
  vmovaps		ymm7, [sin01_8_B_array]
  vpermps		ymm1, ymm6, ymm7
  vaddps       ymm0, ymm0, ymm1
;x = 6.28318530718f * x;
  vmovaps		ymm1, [sin01_8_2pi]
  vmulps       ymm0, ymm0, ymm1
;float preresult = x;
  vmovdqu       ymm1,ymm0
;float dx = x * x;
  vmulps	   ymm2, ymm0, ymm0
;float x3 = dx * x;
  vmulps		ymm3, ymm2, ymm0
;float fx = dx * dx;
  vmulps		ymm4, ymm2, ymm2
;float x5 = fx * x;
  vmulps       ymm5, ymm4, ymm0
;;
  vmovaps		ymm7, [sin01_8_R_array]
  vpermps		ymm0, ymm6, ymm7
;float x7 = fx * x3;
  vmulps       ymm6,ymm4,ymm3
;preresult -= x3 * n3;
  vmovaps		ymm2, [sin01_8_n3]
  vmulps       ymm3, ymm3,ymm2
  vsubps       ymm1, ymm1,ymm3
;float x9 = x5 * fx;
  vmulps       ymm4, ymm4,ymm5
;float temp0 = x5 * n5;
  vmovaps		ymm2, [sin01_8_n5]
  vmulps       ymm5, ymm5,ymm2
;float temp1 = x7 * n7;
  vmovaps		ymm2, [sin01_8_n7]
  vmulps       ymm6, ymm6,ymm2
;preresult += temp0;
  vaddps       ymm1, ymm1,ymm5
;float temp2 = x9 * n9;
  vmovaps		ymm2, [sin01_8_n9]
  vmulps       ymm4, ymm4,ymm2
;preresult -= temp1;
  vsubps       ymm1, ymm1,ymm6
;preresult += temp2;
  vaddps       ymm1, ymm1,ymm4
;return R[index] * preresult;
  vmulps       ymm0, ymm0,ymm1
  vmovups YMMWORD PTR [rcx], ymm0
  ret     0
sin01_8 ENDP

sin01_32 PROC
;x -= (int)x;
  vmovaps      ymm0, [rcx]
  vmovaps      ymm7, [rcx+32]
  vmovaps      ymm14, [rcx+64];
  vmovaps      ymm15, [rcx+96];
  vroundps	   ymm1, ymm0, 1
  vroundps	   ymm8, ymm7, 1
  vroundps	   ymm2, ymm14, 1;
  vroundps	   ymm3, ymm15, 1;
  vsubps       ymm0, ymm0,ymm1
  vsubps       ymm7, ymm7,ymm8
  vsubps       ymm14, ymm14,ymm2;
  vsubps       ymm15, ymm15,ymm3;
;x = 4.0f * x;
  vmovaps		ymm1, [sin01_8_float_4]
  vmulps		ymm0, ymm0, ymm1
  vmulps		ymm7, ymm7, ymm1
  vmulps		ymm14, ymm14, ymm1;
  vmulps		ymm15, ymm15, ymm1;
;ui32 index = (int)x;
  vroundps	   ymm1, ymm0, 1
  vroundps	   ymm8, ymm7, 1
  vroundps	   ymm2, ymm14, 1;
  vroundps	   ymm3, ymm15, 1;
  vcvtps2dq	   ymm6,ymm1
  vcvtps2dq	   ymm13,ymm8
  vcvtps2dq	   ymm4,ymm2;
  vcvtps2dq	   ymm5,ymm3;
;index &= 3; 
;is it require??
  vmovaps		ymm1, [sin01_8_uint3]
  vpand         ymm6, ymm6, ymm1
  vpand         ymm13, ymm13, ymm1
  vpand         ymm4, ymm4, ymm1;
  vpand         ymm5, ymm5, ymm1;
;x = A[index] * x;
  vmovaps		ymm1, [sin01_8_A_array]
  vpermps		ymm8, ymm13, ymm1
  vpermps		ymm2, ymm4, ymm1;
  vpermps		ymm3, ymm5, ymm1;
  vpermps		ymm1, ymm6, ymm1
  vmulps       ymm0, ymm0,ymm1
  vmulps       ymm7, ymm7,ymm8
  vmulps       ymm14, ymm14,ymm2;
  vmulps       ymm15, ymm15,ymm3;
;x += B[index];
  vmovaps		ymm2, [sin01_8_B_array]
  vpermps		ymm1, ymm6, ymm2
  vpermps		ymm8, ymm13, ymm2
  vpermps		ymm9, ymm4, ymm2;
  vpermps		ymm3, ymm5, ymm2;
  vaddps       ymm0, ymm0, ymm1
  vaddps       ymm7, ymm7, ymm8
  vaddps       ymm14, ymm14, ymm9;
  vaddps       ymm15, ymm15, ymm3;
;x = 6.28318530718f * x;
  vmovaps		ymm1, [sin01_8_2pi]
  vmulps       ymm0, ymm0, ymm1
  vmulps       ymm7, ymm7, ymm1
  vmulps       ymm14, ymm14, ymm1;
  vmulps       ymm15, ymm15, ymm1;
  vmovups YMMWORD PTR [rcx+64], ymm4
  vmovups YMMWORD PTR [rcx+96], ymm5
;float preresult = x;
  vmovdqu       ymm1,ymm0
  vmovdqu       ymm8,ymm7
;float dx = x * x;
  vmulps	   ymm2, ymm0, ymm0
  vmulps	   ymm9, ymm7, ymm7
;float x3 = dx * x;
  vmulps		ymm3, ymm2, ymm0
  vmulps		ymm10, ymm9, ymm7
;float fx = dx * dx;
  vmulps		ymm4, ymm2, ymm2
  vmulps		ymm11, ymm9, ymm9
;float x5 = fx * x;
  vmulps       ymm5, ymm4, ymm0
  vmulps       ymm12, ymm11, ymm7
;;
  vmovaps		ymm7, [sin01_8_R_array]
  vpermps		ymm0, ymm6, ymm7
  vpermps		ymm7, ymm13, ymm7
;float x7 = fx * x3;
  vmulps       ymm6,ymm4,ymm3
  vmulps       ymm13,ymm11,ymm10
;preresult -= x3 * n3;
  vmovaps		ymm2, [sin01_8_n3]
  vmulps       ymm3, ymm3,ymm2
  vmulps       ymm10, ymm10,ymm2
  vsubps       ymm1, ymm1,ymm3
  vsubps       ymm8, ymm8,ymm10
;float x9 = x5 * fx;
  vmulps       ymm4, ymm4,ymm5
  vmulps       ymm11, ymm11,ymm12
;float temp0 = x5 * n5;
  vmovaps		ymm2, [sin01_8_n5]
  vmulps       ymm5, ymm5,ymm2
  vmulps       ymm12, ymm12,ymm2
;float temp1 = x7 * n7;
  vmovaps		ymm2, [sin01_8_n7]
  vmulps       ymm6, ymm6,ymm2
  vmulps       ymm13, ymm13,ymm2
;preresult += temp0;
  vaddps       ymm1, ymm1,ymm5
  vaddps       ymm8, ymm8,ymm12
;float temp2 = x9 * n9;
  vmovaps		ymm2, [sin01_8_n9]
  vmulps       ymm4, ymm4,ymm2
  vmulps       ymm11, ymm11,ymm2
;preresult -= temp1;
  vsubps       ymm1, ymm1,ymm6
  vsubps       ymm8, ymm8,ymm13
;preresult += temp2;
  vaddps       ymm1, ymm1,ymm4
  vaddps       ymm8, ymm8,ymm11
;return R[index] * preresult;
  vmulps       ymm0, ymm0,ymm1
  vmulps       ymm7, ymm7,ymm8

  vmovaps      ymm6, [rcx+64];
  vmovaps      ymm13, [rcx+96];
;float preresult = x;
  vmovdqu       ymm1,ymm14
  vmovdqu       ymm8,ymm15
;float dx = x * x;
  vmulps	   ymm2, ymm14, ymm14
  vmulps	   ymm9, ymm15, ymm15
;float x3 = dx * x;
  vmulps		ymm3, ymm2, ymm14
  vmulps		ymm10, ymm9, ymm15
;float fx = dx * dx;
  vmulps		ymm4, ymm2, ymm2
  vmulps		ymm11, ymm9, ymm9
;float x5 = fx * x;
  vmulps       ymm12, ymm11, ymm15
  vmulps       ymm5, ymm4, ymm14
;;
  vmovaps		ymm15, [sin01_8_R_array]
  vpermps		ymm14, ymm6, ymm15
  vpermps		ymm15, ymm13, ymm15
;float x7 = fx * x3;
  vmulps       ymm6,ymm4,ymm3
  vmulps       ymm13,ymm11,ymm10
;preresult -= x3 * n3;
  vmovaps		ymm2, [sin01_8_n3]
  vmulps       ymm3, ymm3,ymm2
  vmulps       ymm10, ymm10,ymm2
  vsubps       ymm1, ymm1,ymm3
  vsubps       ymm8, ymm8,ymm10
;float x9 = x5 * fx;
  vmulps       ymm4, ymm4,ymm5
  vmulps       ymm11, ymm11,ymm12
;float temp0 = x5 * n5;
  vmovaps		ymm2, [sin01_8_n5]
  vmulps       ymm5, ymm5,ymm2
  vmulps       ymm12, ymm12,ymm2
;float temp1 = x7 * n7;
  vmovaps		ymm2, [sin01_8_n7]
  vmulps       ymm6, ymm6,ymm2
  vmulps       ymm13, ymm13,ymm2
;preresult += temp0;
  vaddps       ymm1, ymm1,ymm5
  vaddps       ymm8, ymm8,ymm12
;float temp2 = x9 * n9;
  vmovaps		ymm2, [sin01_8_n9]
  vmulps       ymm4, ymm4,ymm2
  vmulps       ymm11, ymm11,ymm2
;preresult -= temp1;
  vsubps       ymm1, ymm1,ymm6
  vsubps       ymm8, ymm8,ymm13
;preresult += temp2;
  vaddps       ymm1, ymm1,ymm4
  vaddps       ymm8, ymm8,ymm11
;return R[index] * preresult;
  vmulps       ymm14, ymm14,ymm1
  vmulps       ymm15, ymm15,ymm8

  vmovups YMMWORD PTR [rcx], ymm0
  vmovups YMMWORD PTR [rcx+32], ymm7
  vmovups YMMWORD PTR [rcx+64], ymm14
  vmovups YMMWORD PTR [rcx+96], ymm15
  ret     0
sin01_32 ENDP

END