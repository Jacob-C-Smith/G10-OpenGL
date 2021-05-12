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
; size_t  forcesCount  - 0x30
; bool      useGravity   - 0x34
;

_DATA SEGMENT

_DATA ENDS

_CONST SEGMENT
forceOfGravity:
    real4 0.f
    real4 0.f
    real4 0.98f
    real4 0.f
piOver180:
    real4 0.01745329238474369f
    real4 0.01745329238474369f
    real4 0.01745329238474369f
    real4 0.01745329238474369f
oneEightyOverPi:
    real4 57.295772552490234f
    real4 57.295772552490234f
    real4 57.295772552490234f
    real4 57.295772552490234f
_CONST ENDS
 
_TEXT SEGMENT
 
PUBLIC sumVecs

; Summates vector array pointed to by rdx+0x10. Stores resultant vector rdx.
sumVecs PROC                
    push rdx                    ; Save rdx on the stack so we can dereference it later
    add rdx, 10h                ; Start at the array pointed to by rdx+0x10
    vmovaps xmm0, [rdx]         ; Copy out the vector
    loops:                      
        add rdx, 10h            ; Iterate to the next vector
        vmovaps xmm1, [rdx]     ; Copy out the second vector
        vaddps xmm0, xmm0, xmm1 ; Add the vectors together and sotre the result in xmm0

        dec rcx                 ; Decrement the counter
        jrcxz e                 ; If we're done, we exit
        jmp loops               ; If not we keep going
    e:                          ;
    pop rdx                     ; Pop rdx from the stack
    vmovaps [rdx], xmm0         ; Copy xmm0 to memory 
    
    leave
    ret                         ; exit
sumVecs ENDP

; Calculates and sets new acceleration, velocity, and positions
; from a force vector and the mass of the entity. 
;
; velocity (new) = acceleration * delta time + velocity (last)
; position (new) = velocity * delta time + position (last)
calculateDerivativesOfDisplacement PROC
    mov rdx, rcx       ; Copy the pointer to the entity
    mov rdx, [rdx+28h] ; Dereference both of the pointers
    mov rcx, [rcx+30h] 

    mov r9, rcx  ;
    add r9, 28h  ;
    mov r9, [r9] ;

    ; load everything in to xmm registers
    vmovaps xmm0, [r9]			 ; xmm0 = force
    vbroadcastss xmm1, xmm1      ; xmm1 = delta time (scalar)
    vmovaps xmm2, [rcx+20h]      ; xmm2 = mass (scalar)
    vbroadcastss xmm2, xmm2     
    vmovaps  xmm3, xmm0          ; xmm3 = acceleration 
    vdivps  xmm3, xmm3, xmm2     ; force = mass*acceleration => acceleration = force / mass
    vmovaps xmm4, [rcx]          ; xmm4 = last velocity
    vmovaps xmm5, [rdx]			 ; xmm5 = last position

    vfmadd231ps xmm4, xmm1, xmm3 ; last velocity += acceleraiton * delta time
    vmovaps [rcx], xmm4          ; store the result

    vfmadd231ps xmm5, xmm1, xmm4 ; last position += last velocity * delta time 
    vmovaps [rdx], xmm5          ; again store the resukt
    
    leave
    ret                          ; done

calculateDerivativesOfDisplacement ENDP

; Adds two vectors
SSEAddVec PROC
    vmovaps xmm0, [rcx]
    vmovaps xmm1, [rdx]
    vaddps  xmm0, xmm0, xmm1
    vmovaps [r8], xmm0
    leave
    ret
SSEAddVec ENDP

; Subtract two vectors
SSESubVec PROC
    vmovaps xmm0, [rcx]
    vmovaps xmm1, [rdx]
    vsubps  xmm0, xmm0, xmm1
    vmovaps [r8], xmm0
    leave
    ret
SSESubVec ENDP

SSEAddVec3S PROC
    vmovaps xmm0, [rcx]
    vbroadcastss xmm1, xmm1
    vsubps  xmm0, xmm0, xmm1
    vmovaps [r8], xmm0
    leave
    ret
SSEAddVec3S ENDP

SSESubVec3S PROC
    vmovaps xmm0, [rcx]
    vmovaps xmm1, [rdx]
    vsubps  xmm0, xmm0, xmm1
    vmovaps [r8], xmm0
    leave
    ret
SSESubVec3S ENDP

SSEVecxVec PROC

SSEVecxVec ENDP

SSEVecxScalar PROC

SSEVecxScalar ENDP

SSENormalize PROC

SSENormalize ENDP



; Computes cross product of two vectors. Stores result where first parameter points
SSEXProduct PROC
    vmovaps xmm0, [rcx]
    vmovaps xmm1, [rdx]
    vmovaps xmm2, xmm0                 ; copy xmm0 into xmm2
    vmovaps xmm3, xmm1                 ; copy xmm1 into xmm3
    vshufps xmm2,xmm2, xmm2, 11001001b ; packed single shuffle xmm2 with itself
    vshufps xmm3,xmm3, xmm3, 11010010b ; packed single shuffle xmm3 with itself 
    vmovaps xmm4, xmm2                 ; copy xmm2 into xmm4
    vmovaps xmm5, xmm2                 ; copy xmm2 into xmm5
    vmulps  xmm4,xmm4, xmm1            ; multiply xmm4 by xmm1
    vmulps  xmm5,xmm5, xmm3            ; multiply xmm5 by xmm3
    vshufps xmm4,xmm4, xmm4, 11001001b ; packed single shuffle xmm5 with itself
    vsubps  xmm5,xmm5, xmm4            ; subtract xmm4 from xmm5
    vmovaps [r8],xmm5                  ; store result in r8

    leave
    ret
SSEXProduct ENDP

; Transposes inverse matrix
SSETransposeInverseMatrix PROC
    vmovaps xmm0, [rcx]                    ; xmm0 = [ a b c d ]
    vmovaps xmm1, [rcx+16]                 ; xmm1 = [ e f g h ]
    vmovaps xmm2, [rcx+32]                 ; xmm2 = [ i j k l ]
    vmovaps xmm3, [rcx+48]                 ; xmm3 = [ m n o p ]

    vcvtps2pd ymm0, xmm0                   ; Upconvert the single precision floats in xmmN to ymmN
    vcvtps2pd ymm1, xmm1                   ; .
    vcvtps2pd ymm2, xmm2                   ; .
    vcvtps2pd ymm3, xmm3                   ; .

    vunpcklpd ymm4, ymm0, ymm1             ; Here, the rows of the matrix get unpacked into many registers
    vunpckhpd ymm5, ymm0, ymm1             ; .
    vunpcklpd ymm6, ymm2, ymm3             ; .
    vunpckhpd ymm7, ymm2, ymm3             ; .

    vperm2f128 ymm0, ymm4, ymm6, 00100000b ; Here, the unpacked rows get recombined with the packed rows to yelid an inverted matrix
    vperm2f128 ymm1, ymm5, ymm7, 00100000b ; .
    vperm2f128 ymm2, ymm4, ymm6, 00110001b ; .
    vperm2f128 ymm3, ymm5, ymm7, 00110001b ; .

    vcvtpd2ps xmm0, ymm0                   ; Downconvert the double precision floats in ymmN to xmmN
    vcvtpd2ps xmm1, ymm1                   ; .
    vcvtpd2ps xmm2, ymm2                   ; .
    vcvtpd2ps xmm3, ymm3                   ; .

    vmovaps [r8],     xmm0                ; [ a e i m ]
    vmovaps [r8+10h], xmm1                ; [ b f j n ]
    vmovaps [r8+20h], xmm2                ; [ c g k o ]
    vmovaps [r8+30h], xmm3                ; [ d h l p ]

    leave
    ret
SSETransposeInverseMatrix ENDP

SSEMat4xMat4 PROC
        
    ; Save a copy of rcx, rsi, and rdi
    push rcx
    push rsi
    push rdi
    
    mov rsi, rdx
    mov rdi, rcx

    xor rax, rax
    mov rcx, 4
    vzeroall
    l:
    vmovaps xmm0, [rsi]
    
    vbroadcastss xmm1, REAL4 ptr [rdi+rax+0]
    vfmadd231ps xmm2, xmm1, xmm0

    vbroadcastss xmm1, REAL4 ptr [rdi+rax+16]
    vfmadd231ps xmm3, xmm1, xmm0

    vbroadcastss xmm1, REAL4 ptr [rdi+rax+32]
    vfmadd231ps xmm4, xmm1, xmm0

    vbroadcastss xmm1, REAL4 ptr [rdi+rax+48]
    vfmadd231ps xmm5, xmm1, xmm0

    add rax, 4
    add rsi, 16

    loop l

    pop rdi
    pop rsi
    pop rcx

    vmovaps [r8],xmm2
    vmovaps [r8+16],xmm3
    vmovaps [r8+32],xmm4
    vmovaps [r8+48],xmm5

    leave
    ret
SSEMat4xMat4 ENDP

_TEXT ENDS
 
END
