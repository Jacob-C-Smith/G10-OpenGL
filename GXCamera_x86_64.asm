
_DATA SEGMENT

_DATA ENDS

_CONST SEGMENT
allTwo:
    real4 2.f
    real4 2.f
    real4 2.f
    real4 2.f
allOneHalf:
    real4 0.5f
    real4 0.5f
    real4 0.5f
    real4 0.5f
clearUp:
    dword -1
    dword -1
    dword -1
    dword 0
_CONST ENDS
 
_TEXT SEGMENT

; Compute perspective projection, where f = fov, a = aspect, n = near, and r = far
; ?                                                      ?
; ? (a*tan(f/2))^-1, 0,             0,                0, ?
; ? 0,               (tan(f/2))^-1, 0,                0  ?
; ? 0,               0,             -((r+n)/(r-n)),  -1  ?
; ? 0,               0,             -((2*r*n)/(r-n)), 0  ?
; ?                                                      ?
;

; extern void SSEPerspective ( mat4* ret, float fov, float aspect, float near, float far );
PUBLIC AVXPerspective
AVXPerspective PROC
    ;1. Calculate tan(f/2)
    ;vbroadcastss xmm1, xmm1
    ;vbroadcastss xmm4, allOneHalf
    ;vdivps xmm1, xmm1, xmm4
    
    ;2.  

    ;. Calculate r-n
    

    ;. 
    ret
AVXPerspective ENDP

;extern void SSEView ( vec3* eye, vec3* target, vec3* up, mat4* result );        // ? Computes a view matrix from eye, target, and up vectors
;{ 
;	// Compute forward direction
;	vec3 f = normalize((vec3) {
;		eye.x - target.x,
;		eye.y - target.y,
;		eye.z - target.z
;	});
;
;	// Compute left direction as cross product of up and forward
;	vec3 l = normalize(crossProductVec3(up, f));
;
;	// Recompute up
;	vec3 u = crossProductVec3(f, l);
;	
;	// Return the view matrix
;	return (mat4) {
;		l.x, u.x, f.x, 0,
;		l.y, u.y, f.y, 0,
;		l.z, u.z, f.z, 0,
;		-dotProductVec3(l,eye), -dotProductVec3(u, eye), -dotProductVec3(f, eye), 1
;	};
;};

PUBLIC AVXView
AVXView PROC
    ; Load up values
    vmovups xmm0, xmmword ptr [clearUp] ; xmm0 = clear w constant
    vmovups xmm1, xmmword ptr [rcx]     ; xmm1 = eye
    vmovups xmm2, xmmword ptr [rdx]     ; xmm2 = target
    vmovups xmm3, xmmword ptr [r8]      ; xmm3 = up

    ;Clear w, because a nonzero w will interfere with some computations
    vandps xmm1, xmm0, xmm1
    vandps xmm2, xmm0, xmm2
    vandps xmm3, xmm0, xmm3

    vsubps xmm4, xmm1, xmm2 ; xmm4 = forward direction = normalaize(eye - target)

    push rcx                ; 
    push rdx                ; 

;    mov rcx, 

    extern AVXNormalize : PROC
    call AVXNormalize

    pop rdx 
    pop rcx


    ret
AVXView ENDP


END

