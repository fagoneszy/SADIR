global nadir_dot4_sysv
section .text
nadir_dot4_sysv:
    movups xmm0, [rdi]
    movups xmm1, [rsi]
    mulps xmm0, xmm1
    movhlps xmm1, xmm0
    addps xmm0, xmm1
    movaps xmm1, xmm0
    shufps xmm1, xmm1, 1
    addss xmm0, xmm1
    ret
