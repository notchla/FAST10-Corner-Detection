.intel_syntax noprefix

loop:
    mov         rax,qword ptr [rsp+0x0000000000000098]  
    lea         rcx,[rax+r13]  
    mov         rdx,qword ptr [rsp+0x0000000000000090]  
    lea         rax,[rdx+rcx]  
    vmovdqu     ymm0,ymmword ptr [rdx+rcx]  
    vpsubusb    ymm1,ymm0,ymm5  
    vpaddusb    ymm0,ymm5,ymm0  
    mov         rcx,qword ptr [rsp+0x68]  
    vmovdqu     ymm2,ymmword ptr [rcx+rax]  
    mov         rcx,qword ptr [rsp+0x60]  
    vmovdqu     ymm3,ymmword ptr [rcx+rax]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ecx,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   edx,ymm2  
    shl         rdx,0x20  
    or          rdx,rcx  
    not         rdx  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm2,ymm1,ymm2  
    vpmovmskb   ecx,ymm2  
    vpminub     ymm2,ymm3,ymm0  
    vpcmpeqb    ymm2,ymm3,ymm2  
    vpmovmskb   ebp,ymm2  
    shl         rbp,0x20  
    or          rbp,rcx  
    not         rbp  
    mov         rcx,rbp  
    or          rcx,rdx  
    je          loop  
    mov         rcx,qword ptr [rsp+0x50]  
    vmovdqu     ymm2,ymmword ptr [rax+rcx-2]  
    mov         rcx,qword ptr [rsp+0x58]  
    vmovdqu     ymm3,ymmword ptr [rax+rcx+2]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ecx,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   r9d,ymm2  
    shl         r9,0x20  
    or          r9,rcx  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm2,ymm1,ymm2  
    vpmovmskb   ecx,ymm2  
    vpminub     ymm2,ymm3,ymm0  
    vpcmpeqb    ymm2,ymm3,ymm2  
    vpmovmskb   r11d,ymm2  
    shl         r11,0x20  
    or          r11,rcx  
    andn        rcx,r9,rdx  
    andn        r8,r11,rbp  
    or          rcx,r8  
    je          loop  
    lea         rbx,[rax-2]  
    lea         rsi,[rax+2]  
    mov         rdi,qword ptr [rsp+0x58]  
    vmovdqu     ymm3,ymmword ptr [rbx+rdi]  
    mov         rbx,qword ptr [rsp+0x50]  
    vmovdqu     ymm2,ymmword ptr [rsi+rbx]  
    vpminub     ymm4,ymm1,ymm3  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ebx,ymm4  
    vpminub     ymm4,ymm3,ymm0  
    vpcmpeqb    ymm3,ymm3,ymm4  
    vpmovmskb   r10d,ymm3  
    shl         r10,0x20  
    or          r10,rbx  
    andn        r12,r10,rbp  
    not         r10  
    vpminub     ymm3,ymm1,ymm2  
    vpcmpeqb    ymm3,ymm1,ymm3  
    vpmovmskb   ebp,ymm3  
    vpminub     ymm3,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm3  
    vpmovmskb   ebx,ymm2  
    shl         rbx,0x20  
    or          rbx,rbp  
    andn        rbp,rbx,rdx  
    not         rbx  
    or          rbp,r10  
    mov         rsi,r12  
    or          rsi,rbx  
    and         rsi,rcx  
    and         rsi,rbp  
    je          loop  
    not         r9  
    not         r11  
    vmovdqu     ymm3,ymmword ptr [rax-3]  
    vmovdqu     ymm2,ymmword ptr [rax+3]  
    vpminub     ymm4,ymm1,ymm3  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ecx,ymm4  
    vpminub     ymm4,ymm3,ymm0  
    vpcmpeqb    ymm3,ymm3,ymm4  
    vpmovmskb   ebp,ymm3  
    shl         rbp,0x20  
    or          rbp,rcx  
    mov         rcx,r10  
    and         rcx,r9  
    andn        rcx,rbp,rcx  
    mov         rdi,rbp  
    not         rdi  
    vpminub     ymm3,ymm1,ymm2  
    vpcmpeqb    ymm3,ymm1,ymm3  
    vpmovmskb   ebp,ymm3  
    vpminub     ymm3,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm3  
    vpmovmskb   r14d,ymm2  
    shl         r14,0x20  
    or          r14,rbp  
    andn        rbp,r14,rbx  
    not         r14  
    mov         qword ptr [rsp+0x0000000000000080],rbp  
    and         rbp,r11  
    mov         qword ptr [rsp+0x78],rdi  
    or          rbp,rdi  
    or          rcx,r14  
    and         rcx,rsi  
    and         rcx,rbp  
    je          loop  
    mov         rbp,qword ptr [rsp+0x68]  
    vmovdqu     ymm2,ymmword ptr [rax+rbp-1]  
    vmovdqu     ymm3,ymmword ptr [rax+rbp+1]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   esi,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   edi,ymm2  
    shl         rdi,0x20  
    or          rdi,rsi  
    not         rdi  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm2,ymm1,ymm2  
    vpmovmskb   esi,ymm2  
    vpminub     ymm2,ymm3,ymm0  
    vpcmpeqb    ymm2,ymm3,ymm2  
    vpmovmskb   r15d,ymm2  
    shl         r15,0x20  
    or          r15,rsi  
    not         r15  
    or          r8,rdi  
    and         r8,rcx  
    or          r12,r15  
    and         r12,r8  
    je          loop  
    lea         rcx,[rax-1]  
    lea         rbp,[rax+1]  
    mov         rsi,qword ptr [rsp+0x60]  
    vmovdqu     ymm2,ymmword ptr [rcx+rsi]  
    vmovdqu     ymm3,ymmword ptr [rbp+rsi]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ebp,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   ecx,ymm2  
    shl         rcx,0x20  
    or          rcx,rbp  
    not         rcx  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm2,ymm1,ymm2  
    vpmovmskb   esi,ymm2  
    vpminub     ymm2,ymm3,ymm0  
    vpcmpeqb    ymm2,ymm3,ymm2  
    vpmovmskb   r8d,ymm2  
    shl         r8,0x20  
    or          r8,rsi  
    not         r8  
    and         rdx,rdi  
    and         rdx,r15  
    and         rbx,rdx  
    or          rbx,rcx  
    and         rbx,r12  
    and         rdx,r9  
    or          rdx,r8  
    and         rdx,rbx  
    je          loop  
    lea         rsi,[rax-3]  
    mov         rbx,qword ptr [rsp+0x40]  
    vmovdqu     ymm2,ymmword ptr [rsi+rbx]  
    mov         rbx,qword ptr [rsp+0x48]  
    vmovdqu     ymm3,ymmword ptr [rsi+rbx]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   ebx,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   esi,ymm2  
    shl         rsi,0x20  
    or          rsi,rbx  
    not         rsi  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm2,ymm1,ymm2  
    vpmovmskb   ebp,ymm2  
    vpminub     ymm2,ymm3,ymm0  
    vpcmpeqb    ymm2,ymm3,ymm2  
    vpmovmskb   ebx,ymm2  
    shl         rbx,0x20  
    or          rbx,rbp  
    not         rbx  
    and         r14,r11  
    and         r14,r8  
    or          r14,rsi  
    and         r14,rdx  
    mov         r8,qword ptr [rsp+0x0000000000000080]  
    and         r8,r15  
    or          r8,rbx  
    and         r8,r14  
    je          loop  
    add         rax,3  
    mov         rdx,qword ptr [rsp+0x40]  
    vmovdqu     ymm2,ymmword ptr [rax+rdx]  
    mov         rdx,qword ptr [rsp+0x48]  
    vmovdqu     ymm3,ymmword ptr [rax+rdx]  
    vpminub     ymm4,ymm1,ymm2  
    vpcmpeqb    ymm4,ymm1,ymm4  
    vpmovmskb   eax,ymm4  
    vpminub     ymm4,ymm2,ymm0  
    vpcmpeqb    ymm2,ymm2,ymm4  
    vpmovmskb   edx,ymm2  
    shl         rdx,0x20  
    or          rdx,rax  
    not         rdx  
    vpminub     ymm2,ymm1,ymm3  
    vpcmpeqb    ymm1,ymm1,ymm2  
    vpmovmskb   eax,ymm1  
    vpminub     ymm0,ymm3,ymm0  
    vpcmpeqb    ymm0,ymm3,ymm0  
    vpmovmskb   ebp,ymm0  
    shl         rbp,0x20  
    or          rbp,rax  
    not         rbp  
    and         rsi,qword ptr [rsp+0x78]  
    and         rsi,rbx  
    and         rcx,r10  
    and         rcx,rsi  
    or          rcx,rdx  
    and         rcx,r8  
    and         rdi,r9  
    and         rdi,rsi  
    or          rdi,rbp  
    and         rdi,rcx  
    je          loop  
    mov         rsi,rdi  
    shr         rsi,0x20  
    or          rsi,rdi  
    xor         edi,edi  
    mov         rax,qword ptr [rsp+0x00000000000000C8]  
    mov         qword ptr [rdx],rax  
    add         qword ptr [rcx+8],8  
    jmp         loop  

