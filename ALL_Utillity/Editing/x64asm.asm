.code

; ���ν����� ù��° �Ķ���ʹ� �׻� c�� �޴´�.
; �� �������� masm�� ��¥ asm�� �ƴ϶� �׳� ���� �����ڵ尰���Ŷ� �����ȵǴ� ������ �ֱ��� ������.

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

END