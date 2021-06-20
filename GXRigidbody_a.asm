
;	x86 calling conventions
;	rcx/xmm0
;	rdx/xmm1
;	r8/xmm2
;	r9/xmm3
;	RTL (C)
;
;	Stack aligned on 16 bytes. 32 bytes shadow space on stack. 
;	The specified 8 registers can only be used for parameters 
;	1 through 4. For C++ classes, the hidden this parameter 
;	is the first parameter, and is passed in RCX.
;

; 
; Offset for GXEntity_t
;
; GXvec3_t  velocity     - 0x00
; GXvec3_t  acceleration - 0x10
; float     mass         - 0x20
; GXvec3_t* forces       - 0x28
; GXsize_t  forcesCount  - 0x30
; bool      useGravity   - 0x34
;

_DATA SEGMENT
forceOfGravity:
	real4 0.f
	real4 0.f
	real4 9.8f
	real4 0.f
_DATA ENDS
 
_TEXT SEGMENT
 
PUBLIC sumVecs
 
sumVecs PROC                ; Summates vectors
	movaps xmm0, [rdx]
	push rdx
	loops:
		add rdx, 10h
		movaps xmm1, [rdx]
		addps xmm0, xmm1

		dec rcx
		jrcxz e
		jmp loops
	e:
	pop rdx
	movntps [rdx], xmm0
	ret
sumVecs ENDP

;
; void calculateDerivativesOfDisplacement ( GXEntity_t* entity );
; 
; Calculates and sets new acceleration, velocity, and positions
; from a force vector and the mass of the entity. 
;

;
; velocity = acceleration * delta time + velocity (last)
; position = velocity * delta time + position (last)
;
calculateDerivativesOfDisplacement PROC
	mov rdx, rcx       ; Copy the pointer to the entity
	mov rdx, [rdx+28h] ; Dereference both of the pointers
	mov rcx, [rcx+30h] 

	mov r9, rcx
	add r9, 28h
	mov r9, [r9]

	; load everything in to xmm registers
	movaps xmm0, [r9]			 ; xmm0 = force
	vbroadcastss xmm1, xmm1      ; xmm1 = delta time (scalar)
	movaps xmm2, [rcx+20h]       ; xmm2 = mass (scalar)
	vbroadcastss xmm2, xmm2     
	movaps xmm3, xmm0            ; xmm3 = acceleration 
	divps  xmm3, xmm2            ; force = mass*acceleration => acceleration = force / mass
	movaps xmm4, [rcx]           ; xmm4 = last velocity
	movaps xmm5, [rdx]			 ; xmm5 = last position
	 
	vfmadd231ps xmm4, xmm1, xmm3 ; velocity = acceleration * deltaTime + lastVelocity
	movntps [rcx], xmm4          ; nontemporal store to  memory
	
	vfmadd231ps xmm5, xmm1, xmm4 ; position = velocity * deltaTime + last positioon
	movntps [rdx], xmm5          ; nontemporal store to m
	ret

calculateDerivativesOfDisplacement ENDP

sumForcesAndCalculateDisplacements PROC
	xchg rax,rax
sumForcesAndCalculateDisplacements ENDP

_TEXT ENDS
 
END
