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

nppp:
    real4  1.f
    real4  1.f
    real4  1.f
    real4 -1.f

nnnn:
    real4 -1.f
    real4 -1.f
    real4 -1.f
    real4 -1.f

;    a: 
;    DWORD 00h
;    b: 
;    DWORD 1Bh
;    c: 
;    DWORD 5Bh  
;    d: 
;    DWORD 40h   
;    e: 
;    DWORD ADh
;    f: 
;    DWORD B6h
;    g: 
;    DWORD F6h   
;    h: 
;    DWORD EDh 

_CONST ENDS

_TEXT SEGMENT

;
; Quaternion multiplication
;

; [w1]  * [w2]  = [ w1*w2 - x1*x2 - y1*y2 - z1*z2 ],
; [x1]i * [x2]i = [ w1*x2 + x1*w2 + y1*z2 - z1*y2 ]i,
; [y1]j * [y2]j = [ w1*y2 + y1*w2 + z1*x2 - x1*z2 ]j,
; [z1]k * [z2]k = [ w1*z2 + z1*w2 + x1*y2 - y1*x2 ]k

extern AVXHorizontalAdd : PROC

PUBLIC AVXQMul
AVXQMul PROC
    vmovups xmm0, [rcx]
    vmovups xmm1, [rdx]
    
    ; xmm2 = A
    vshufps xmm2, xmm0, xmm0, 00h ; xmm2 = [ w1, w1, w1, w1 ]
    vmovups xmm3, xmm1            ; xmm3 = [ w2, x2, y2, z2 ] 
    vmulps  xmm2, xmm2, xmm3      ; xmm2 = xmm2 * xmm3
    
    ; xmm3 = B
    vshufps xmm3, xmm0, xmm0, 91 ; xmm3 = [ x1, x1, y1, z1 ] 
    vshufps xmm4, xmm1, xmm1, 64 ; xmm4 = [ x2, w2, w2, w2 ]
    vmulps  xmm3, xmm3, xmm4     ; xmm3 = xmm3 * xmm4

    ; xmm4 = C
    vshufps xmm4, xmm0, xmm0, 173 ; xmm4 = [ x1, x1, y1, z1 ] 
    vshufps xmm5, xmm1, xmm1, 182 ; xmm5 = [ x2, w2, w2, w2 ]
    vmulps  xmm4, xmm4, xmm5      ; xmm4 = xmm4 * xmm5

    ; xmm5 = D
    vshufps xmm5, xmm0, xmm0, 246 ; xmm3 = [ x1, x1, y1, z1 ] 
    vshufps xmm1, xmm1, xmm1, 237 ; xmm4 = [ x2, w2, w2, w2 ]
    vmulps  xmm5, xmm5, xmm1      ; xmm3 = xmm3 * xmm4

    ; B = B * [ -1, 1, 1, 1 ] 
    vmulps  xmm3, xmm3, xmmword ptr [nppp]

    ; C = C * [ -1, 1, 1, 1 ]
    vmulps  xmm4, xmm4, xmmword ptr [nppp]

    ; D = D * [ -1, -1, -1, -1 ]
    vmulps  xmm5, xmm5, xmmword ptr [nnnn]

    ; xmm0 =  A * B * C * D
    vaddps  xmm2, xmm2, xmm3
    vaddps  xmm2, xmm2, xmm4
    vaddps  xmm2, xmm2, xmm5

    vshufps xmm2, xmm2, xmm2, 27

    vmovups [r8], xmm2

    ret
AVXQMul ENDP

; TODO: Quaternion spherical linear interpolation
;PUBLIC AVXQSlerp
;AVXQSlerp PROC
    ; slerp(xmmword q0, xmmword q1,float dT) = q0 * ( q0^-1 * q1 ) ^ dT

;    ret ; exit
;AVXQSlerp ENDP

;PUBLIC AVXdQSlerp
;AVXdQSlerp PROC
    
;    ret ;
;AVXdQSlerp ENDP

_TEXT ENDS
 
END
