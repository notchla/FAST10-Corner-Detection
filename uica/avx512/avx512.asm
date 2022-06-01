.intel_syntax noprefix

loop:
    mov         rax,qword ptr [rsp+0x50]  
    add         rax,0x40
    add         r14,0x40
    mov         qword ptr [rsp+0x50],rax  
    cmp         rax,qword ptr [rsp+0x0000000000000108]  
    jge         loop  
    mov         rax,qword ptr [rsp+0x0000000000000118]  
    mov         rcx,qword ptr [rsp+0x50]  
    add         rcx,rax  
    mov         rax,qword ptr [rsp+0x0000000000000110]  
    lea         r10,[rax+rcx]  
    vmovdqu64   zmm1,zmmword ptr [rax+rcx]  
    vpsubusb    zmm0,zmm1,zmm5  
    vpaddusb    zmm1,zmm5,zmm1  
    mov         rax,qword ptr [rsp+0x00000000000000C0]  
    vmovdqu64   zmm2,zmmword ptr [rax+r10]  
    mov         rax,qword ptr [rsp+0x00000000000000B8]  
    vmovdqu64   zmm3,zmmword ptr [rax+r10]  
    vpcmpub     k0,zmm0,zmm2,2  
    kmovq       rcx,k0  
    vpcmpub     k0,zmm2,zmm1,2  
    kmovq       rsi,k0  
    vmovq       xmm2,rsi  
    vmovq       xmm4,rcx  
    vpunpcklqdq xmm2,xmm4,xmm2  
    vpcmpub     k0,zmm0,zmm3,2  
    kmovq       r15,k0  
    vpcmpub     k0,zmm3,zmm1,2  
    kmovq       r9,k0  
    vmovq       xmm3,r9  
    vmovq       xmm4,r15  
    vpunpcklqdq xmm3,xmm4,xmm3  
    vpternlogq  xmm3,xmm3,xmm3,0x0F  
    vpternlogq  xmm3,xmm2,xmm6,0x0F6  
    vptest      xmm3,xmm3  
    je          loop