
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
; size_t    forcesCount  - 0x30
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

PUBLIC AVXCollision
AVXCollision PROC
    
boxBox:
    mov r8, rdx                   ; Copy the pointer to the second entity
    mov r9, rdx

    mov rdx, rcx

    ; Dereference both of the pointers
    mov rdx, [rdx+28h]           ; rdx = a transform
    mov rcx, [rcx+38h]           ; rcx = a collider
    mov r8, [r8+28h]             ; r8  = b transform
    mov r9, [r9+38h]             ; r9  = b collider

    vmovups xmm0, [rdx]          ; xmm0 = a location
    vmovaps xmm1, xmm0           ; xmm1 = a location

    vmovups xmm2, [r8]           ; xmm2 = b location
    vmovaps xmm3, xmm2           ; xmm3 = b location

    vaddps  xmm1, xmm0, [rcx+4h] ; xmm1 = a max vector
    vsubps  xmm0, xmm0, [rcx+4h] ; xmm0 = a min vector

    vaddps xmm3, xmm2, [r9+4h]   ; xmm3 = b max vector
    vsubps xmm2, xmm2, [r9+4h]   ; xmm4 = b min vector

    vcmple_oqps xmm4, xmm0, xmm3 ; xmm4 = a min < b max
    vcmpge_oqps xmm5, xmm1, xmm2 ; xmm5 = a max > b min

    ptest xmm4, xmm5             ; carry flag is set if is set if xmm4 AND NOT xmm5 == 0

    jc retTrue                   ; If the carry flag is set, we have a collision
    jmp retFalse

    ret

boxSphere:
    mov r8, rdx                   ; Copy the pointer to the second entity
    mov r9, rdx

    mov rdx, rcx

    ; Dereference both of the pointers
    mov rdx, [rdx+28h]           ; rdx  = ptr a transform
    mov rcx, [rcx+38h]           ; rcx  = ptr a collider
    mov r8, [r8+28h]             ; r8   = ptr b transform
    mov r9, [r9+38h]             ; r9   = ptr b collider

    ; Get the location of the box
    vmovups xmm0, [rdx]          ; xmm0 = a location
    vmovaps xmm1, xmm0           ; xmm1 = a location

    ; Get the location of the sphere, and load the radius
    vmovups xmm2, [r8]                ; xmm2 = b location
;    vmovss  xmm3, xmm3, dword [r9+4h] ; xmm3 = b radius

    ; Compute the minimum and maximum vectors for the box collider
    vaddps  xmm1, xmm0, [rcx+4h] ; xmm1 = a max vector
    vsubps  xmm0, xmm0, [rcx+4h] ; xmm0 = a min vector

    ; Find the closest point to the sphere
    vminps xmm4, xmm3, xmm1       ; xmm4 = min(box max, sphere location)
    vmaxps xmm4, xmm0, xmm4       ; xmm4 = max(box min, min(box max, sphere location))

    ; Check to see if the point is inside the sphere
    vsubps xmm5, xmm2, xmm4 
    vmulps xmm5, xmm5, xmm5

    vcmple_oqps xmm4, xmm0, xmm3 ; xmm4 = a min < b max
    vcmpge_oqps xmm5, xmm1, xmm2 ; xmm5 = a max > b min

    ptest xmm4, xmm5             ; carry flag is set if is set if xmm4 AND NOT xmm5 == 0

    jc retTrue                   ; If the carry flag is set, we have a collision
    jmp retFalse


    ret

boxCapsule:
boxCylinder:
boxCone:

sphereSphere:
sphereCapsule:
sphereCylinder:
sphereCone:

capsuleCapsule:
capsuleCylinder:
capsuleCone:

cylinderCylinder:
cylinderCone:

coneCone:

retTrue:                     ; If so, return 1
    mov rax, 1
    ret
retFalse:
    xor rax, rax
    ret

AVXCollision ENDP


_TEXT ENDS
 
END


