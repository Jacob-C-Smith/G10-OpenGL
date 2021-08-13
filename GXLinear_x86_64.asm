_DATA SEGMENT

_DATA ENDS

_CONST SEGMENT
one3zero:
    real4 1.0
    real4 0.0
    real4 0.0
    real4 0.0
negativeone:
    real4 -1.0
one:
    real4 1.0
alltwo:
    real4 2.0
    real4 2.0
    real4 2.0
    real4 2.0
cloneData:
    dword 01
    dword 01
    dword 02
    dword 02
    dword 03
    dword 03
    dword 00
    dword 00
ppnn:
    real4 1.0
    real4 1.0
    real4 -1.0
    real4 -1.0
pnpn:
    real4 1.0
    real4 -1.0
    real4 1.0
    real4 -1.0
pnnp:
    real4 1.0
    real4 -1.0
    real4 -1.0
    real4 1.0
topPart:
    dword 4
    dword 2
    dword 1
    dword 4
    dword 3
    dword 4
    dword 6
    dword 4
bottomPart:
    dword 0
    dword 7
    dword 4
    dword 4
    dword 4
    dword 4
    dword 4
    dword 4
_CONST ENDS
 
_TEXT SEGMENT
    PUBLIC AVXIdentityMat
    AVXIdentityMat PROC
        vmovaps xmm0, xmmword ptr [one3zero]
        vmovaps [rcx],    xmm0
        vshufps xmm0, xmm0, xmm0, 01010001b
        vmovaps [rcx+16], xmm0
        vshufps xmm0, xmm0, xmm0, 10011010b
        vmovaps [rcx+32], xmm0
        vshufps xmm0, xmm0, xmm0, 10111111b
        vmovaps [rcx+48], xmm0
        
        ret
    AVXIdentityMat ENDP

    PUBLIC AVXTranslationMat 
    AVXTranslationMat PROC
        vmovaps xmm0, xmmword ptr [one3zero]
        vmovaps [rcx],    xmm0
        vshufps xmm0, xmm0, xmm0, 01010001b
        vmovaps [rcx+16], xmm0
        vshufps xmm0, xmm0, xmm0, 10011010b
        vmovaps [rcx+32], xmm0
        vmovups xmm0, xmmword ptr [rdx]
        vinsertps xmm0, xmm0, real4 ptr [one3zero], 00110000b
        vmovaps [rcx+48], xmm0

        ret
    AVXTranslationMat ENDP


    PUBLIC AVXRotationMat    
    AVXRotationMat PROC 
        vmovups xmm0, xmmword ptr [rdx]            ; xmm0 = [ k , j , i , w ]

        vshufps xmm1, xmm0, xmm0, 00010110b        ; xmm1 = [ w, i, i, j ]
        vshufps xmm2, xmm0, xmm0, 00101111b        ; xmm2 = [ w, j, k, k ]
        vshufps xmm3, xmm0, xmm0, 00111001b        ; xmm3 = [ w, k, j, i ]
        vshufps xmm4, xmm0, xmm0, 00000000b        ; xmm4 = [ w, w, w, w ]
         
        vmulps  xmm1, xmm1, xmm2                   ; xmm1 = [ ww, ij, ik, jk ]
        vmulps  xmm2, xmm3, xmm4                   ; xmm2 = [ ww, kw, jw, iw ] 

        vmulps  xmm1, xmm1, xmmword ptr [alltwo]   ; xmm1 = [ 2ww, 2ij, 2ik, 2jk ] 
        vmulps  xmm2, xmm2, xmmword ptr [alltwo]   ; xmm2 = [ 2ww, 2kw, 2jw, 2iw ] 

        vmovups ymm3, ymmword ptr [cloneData]      ; ymm3 = [ 0, 0, 3, 3, 2, 2, 1, 1 ]
        
        vpermps ymm1, ymm3, ymm1                   ; ymm0 = [ 0, 0, 2ij, 2ij, 2ik, 2ik, 2jk, 2jk ]
        vpermps ymm2, ymm3, ymm2                   ; ymm1 = [ 0, 0, 2kw, 2kw, 2jw, 2jw, 2iw, 2iw ]
        
        vaddsubps ymm3, ymm1, ymm2                 ; ymm3 = [ j, g, 0, 0, e, b, c, i ]

        vmovups ymm2, ymmword ptr [topPart]
        vpermps ymm4, ymm2, ymm3 
        vmovups [rcx], ymm4

        vmovups ymm2, ymmword ptr [bottomPart]
        vpermps ymm5, ymm2, ymm3
        vmovups [rcx+32], ymm5

        vmovaps xmm2, xmm0                         ; xmm2 = [ k , j , i , w ]
        vmulps  xmm2, xmm2, xmm2                   ; xmm2 = [ kk , jj , ii , ww ]

        
        vmovss xmm0, real4 ptr [one]
        vmovss real4 ptr [rcx+60], xmm0

        ret
    AVXRotationMat ENDP

    PUBLIC AVXScaleMat       
    AVXScaleMat PROC

    AVXScaleMat ENDP

_TEXT ENDS
 
END
