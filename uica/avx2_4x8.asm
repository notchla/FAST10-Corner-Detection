.intel_syntax noprefix

loop:
    mov         rax,qword ptr [rsp+0x0000000000000088]  
    lea         rbx,[rax+r13]  
    add         rbx,qword ptr [rsp+0x0000000000000080]  
    vpcmpeqd    ymm0,ymm0,ymm0  
    vpxor       xmm1,xmm1,xmm1  
    vpgatherdd  ymm1,dword ptr [rbx+ymm5],ymm0  
    vmovdqu     ymm2,ymmword ptr [rsp+0x00000000000000B0]  
    vpsubusb    ymm0,ymm1,ymm2  
    vpaddusb    ymm1,ymm2,ymm1  
    mov         rax,qword ptr [rsp+0x58]  
    add         rax,rbx  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rax+ymm5],ymm2  
    mov         rax,qword ptr [rsp+0x50]  
    add         rax,rbx  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rax+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       eax,k0  
    kmovd       ecx,k1  
    shl         rcx,0x20  
    or          rcx,rax  
    not         rcx  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       eax,k0  
    kmovd       edx,k1  
    shl         rdx,0x20  
    or          rdx,rax  
    not         rdx  
    mov         rax,rdx  
    or          rax,rcx  
    je          loop  
    mov         rax,qword ptr [rsp+0x40]  
    add         rax,rbx  
    add         rax,0x0FFFFFFFFFFFFFFFE  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rax+ymm5],ymm2  
    mov         rax,qword ptr [rsp+0x48]  
    lea         rax,[rbx+rax+2]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rax+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       eax,k0  
    kmovd       r12d,k1  
    shl         r12,0x20  
    or          r12,rax  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       eax,k0  
    kmovd       r10d,k1  
    shl         r10,0x20  
    or          r10,rax  
    andn        rax,r12,rcx  
    andn        r8,r10,rdx  
    or          rax,r8  
    je          loop  
    lea         rbp,[rbx-2]  
    lea         rsi,[rbx+2]  
    add         rbp,qword ptr [rsp+0x48]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rbp+ymm5],ymm2  
    add         rsi,qword ptr [rsp+0x40]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rsi+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       ebp,k0  
    kmovd       r11d,k1  
    shl         r11,0x20  
    or          r11,rbp  
    andn        r14,r11,rdx  
    not         r11  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       ebp,k0  
    kmovd       esi,k1  
    shl         rsi,0x20  
    or          rsi,rbp  
    andn        rdx,rsi,rcx  
    not         rsi  
    or          rdx,r11  
    mov         rbp,r14  
    or          rbp,rsi  
    and         rbp,rax  
    and         rbp,rdx  
    je          loop  
    not         r12  
    not         r10  
    lea         rax,[rbx-3]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    mov         qword ptr [rsp+0x70],rax  
    vpgatherdd  ymm3,dword ptr [rax+ymm5],ymm2  
    lea         rax,[rbx+3]  
    vpcmpeqd    ymm4,ymm4,ymm4  
    vpxor       xmm2,xmm2,xmm2  
    mov         qword ptr [rsp+0x68],rax  
    vpgatherdd  ymm2,dword ptr [rax+ymm5],ymm4  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       eax,k0  
    kmovd       edx,k1  
    shl         rdx,0x20  
    or          rdx,rax  
    mov         rax,r11  
    and         rax,r12  
    andn        rax,rdx,rax  
    not         rdx  
    vpcmpub     k0,ymm0,ymm2,2  
    vpcmpub     k1,ymm1,ymm2,5  
    kmovd       edi,k0  
    kmovd       r15d,k1  
    shl         r15,0x20  
    or          r15,rdi  
    andn        rdi,r15,rsi  
    not         r15  
    mov         qword ptr [rsp+0x78],rdi  
    and         rdi,r10  
    or          rdi,rdx  
    or          rax,r15  
    and         rax,rbp  
    and         rax,rdi  
    je          loop  
    mov         rbp,qword ptr [rsp+0x58]  
    add         rbp,rbx  
    add         rbp,0x0FFFFFFFFFFFFFFFF  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rbp+ymm5],ymm2  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rbp+ymm5+2],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       edi,k0  
    kmovd       ebp,k1  
    shl         rbp,0x20  
    or          rbp,rdi  
    not         rbp  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       edi,k0  
    kmovd       r9d,k1  
    shl         r9,0x20  
    or          r9,rdi  
    not         r9  
    or          r8,rbp  
    and         r8,rax  
    or          r14,r9  
    and         r14,r8  
    je          loop  
    lea         rax,[rbx-1]  
    add         rbx,1  
    mov         rdi,qword ptr [rsp+0x50]  
    add         rax,rdi  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rax+ymm5],ymm2  
    add         rbx,rdi  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rbx+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       eax,k0  
    kmovd       r8d,k1  
    shl         r8,0x20  
    or          r8,rax  
    not         r8  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       eax,k0  
    kmovd       ebx,k1  
    shl         rbx,0x20  
    or          rbx,rax  
    not         rbx  
    and         rcx,rbp  
    and         rcx,r9  
    and         rsi,rcx  
    or          rsi,r8  
    and         rsi,r14  
    and         rcx,r12  
    or          rcx,rbx  
    and         rcx,rsi  
    je          loop  
    mov         rax,qword ptr [rsp+0x30]  
    mov         rsi,qword ptr [rsp+0x70]  
    add         rax,rsi  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rax+ymm5],ymm2  
    add         rsi,qword ptr [rsp+0x38]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rsi+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       eax,k0  
    kmovd       esi,k1  
    shl         rsi,0x20  
    or          rsi,rax  
    not         rsi  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       edi,k0  
    kmovd       eax,k1  
    shl         rax,0x20  
    or          rax,rdi  
    not         rax  
    and         r15,r10  
    and         r15,rbx  
    or          r15,rsi  
    and         r15,rcx  
    mov         rcx,qword ptr [rsp+0x78]  
    and         rcx,r9  
    or          rcx,rax  
    and         rcx,r15  
    je          loop  
    mov         r9,rcx  
    mov         rcx,qword ptr [rsp+0x30]  
    mov         rbx,qword ptr [rsp+0x68]  
    add         rcx,rbx  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm3,xmm3,xmm3  
    vpgatherdd  ymm3,dword ptr [rcx+ymm5],ymm2  
    add         rbx,qword ptr [rsp+0x38]  
    vpcmpeqd    ymm2,ymm2,ymm2  
    vpxor       xmm4,xmm4,xmm4  
    vpgatherdd  ymm4,dword ptr [rbx+ymm5],ymm2  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm1,ymm3,5  
    kmovd       ecx,k0  
    kmovd       ebx,k1  
    shl         rbx,0x20  
    or          rbx,rcx  
    not         rbx  
    vpcmpub     k0,ymm0,ymm4,2  
    vpcmpub     k1,ymm1,ymm4,5  
    kmovd       ecx,k0  
    kmovd       edi,k1  
    shl         rdi,0x20  
    or          rdi,rcx  
    not         rdi  
    and         rsi,rdx  
    and         rsi,rax  
    and         r8,r11  
    and         r8,rsi  
    or          r8,rbx  
    and         r8,r9  
    and         rbp,r12  
    and         rbp,rsi  
    or          rbp,rdi  
    and         rbp,r8  
    je          loop  
    mov         rax,qword ptr [rsp+0x28]  
    mov         edi,eax  
    xor         ebx,ebx  
    nop         word ptr [rax+rax+0x0000000000000000]  
    mov         rax,qword ptr [rsp+0x00000000000000D8]  
    mov         qword ptr [rdx],rax  
    add         qword ptr [rbp+8],8  
    add         rbx,4  
    add         edi,1  
    cmp         rbx,0x20  
    jmp         loop  

