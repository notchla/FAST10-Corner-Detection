.intel_syntax noprefix

loop:
    mov         rax,qword ptr [rsp+0x0000000000000098]  
    lea         rcx,[rax+r13]  
    mov         rdx,qword ptr [rsp+0x0000000000000090]  
    lea         rax,[rdx+rcx]  
    vmovdqa     xmm0,xmmword ptr [rdx+rcx]  
    vpsubusb    xmm1,xmm0,xmm6  
    vpaddusb    xmm0,xmm6,xmm0  
    mov         rcx,qword ptr [rsp+0x70]  
    vmovdqa     xmm2,xmmword ptr [rcx+rax]  
    mov         rcx,qword ptr [rsp+0x68]  
    vmovdqa     xmm3,xmmword ptr [rcx+rax]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ecx,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   edx,xmm2  
    shl         edx,0x10  
    or          edx,ecx  
    not         edx  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   ecx,xmm2  
    vpminub     xmm2,xmm3,xmm0  
    vpcmpeqb    xmm2,xmm3,xmm2  
    vpmovmskb   ebp,xmm2  
    shl         ebp,0x10  
    or          ebp,ecx  
    not         ebp  
    mov         ecx,ebp  
    or          ecx,edx  
    je          loop  
    mov         rcx,qword ptr [rsp+0x58]  
    vmovdqu     xmm2,xmmword ptr [rax+rcx-2]  
    mov         rcx,qword ptr [rsp+0x60]  
    vmovdqu     xmm3,xmmword ptr [rax+rcx+2]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ecx,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   r9d,xmm2  
    shl         r9d,0x10  
    or          r9d,ecx  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   ecx,xmm2  
    vpminub     xmm2,xmm3,xmm0  
    vpcmpeqb    xmm2,xmm3,xmm2  
    vpmovmskb   r11d,xmm2  
    shl         r11d,0x10  
    or          r11d,ecx  
    andn        ecx,r9d,edx  
    andn        r8d,r11d,ebp  
    or          ecx,r8d  
    je          loop  
    lea         rbx,[rax-2]  
    lea         rsi,[rax+2]  
    mov         rdi,qword ptr [rsp+0x60]  
    vmovdqu     xmm2,xmmword ptr [rbx+rdi]  
    mov         rbx,qword ptr [rsp+0x58]  
    vmovdqu     xmm3,xmmword ptr [rsi+rbx]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ebx,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   r10d,xmm2  
    shl         r10d,0x10  
    or          r10d,ebx  
    andn        r12d,r10d,ebp  
    not         r10d  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   ebp,xmm2  
    vpminub     xmm2,xmm3,xmm0  
    vpcmpeqb    xmm2,xmm3,xmm2  
    vpmovmskb   ebx,xmm2  
    shl         ebx,0x10  
    or          ebx,ebp  
    andn        ebp,ebx,edx  
    not         ebx  
    or          ebp,r10d  
    mov         esi,r12d  
    or          esi,ebx  
    and         esi,ecx  
    and         esi,ebp  
    je          loop  
    not         r9d  
    not         r11d  
    vmovdqu     xmm3,xmmword ptr [rax-3]  
    vmovdqu     xmm2,xmmword ptr [rax+3]  
    vpminub     xmm4,xmm1,xmm3  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ecx,xmm4  
    vpminub     xmm4,xmm3,xmm0  
    vpcmpeqb    xmm3,xmm3,xmm4  
    vpmovmskb   ebp,xmm3  
    shl         ebp,0x10  
    or          ebp,ecx  
    mov         ecx,r10d  
    and         ecx,r9d  
    andn        ecx,ebp,ecx  
    mov         edi,ebp  
    not         edi  
    vpminub     xmm3,xmm1,xmm2  
    vpcmpeqb    xmm3,xmm1,xmm3  
    vpmovmskb   ebp,xmm3  
    vpminub     xmm3,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm3  
    vpmovmskb   r15d,xmm2  
    shl         r15d,0x10  
    or          r15d,ebp  
    andn        ebp,r15d,ebx  
    not         r15d  
    mov         dword ptr [rsp+0x3C],ebp  
    and         ebp,r11d  
    mov         dword ptr [rsp+0x38],edi  
    or          ebp,edi  
    or          ecx,r15d  
    and         ecx,esi  
    and         ecx,ebp  
    je          loop  
    mov         rbp,qword ptr [rsp+0x70]  
    vmovdqu     xmm2,xmmword ptr [rax+rbp-1]  
    mov         rsi,qword ptr [rsp+0x0000000000000080]  
    vmovdqu     xmm3,xmmword ptr [rax+rsi]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   esi,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   edi,xmm2  
    shl         edi,0x10  
    or          edi,esi  
    not         edi  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   esi,xmm2  
    vpminub     xmm2,xmm3,xmm0  
    vpcmpeqb    xmm2,xmm3,xmm2  
    vpmovmskb   r14d,xmm2  
    shl         r14d,0x10  
    or          r14d,esi  
    not         r14d  
    or          r8d,edi  
    and         r8d,ecx  
    or          r12d,r14d  
    and         r12d,r8d  
    je          loop  
    mov         rcx,qword ptr [rsp+0x68]  
    vmovdqu     xmm2,xmmword ptr [rax+rcx-1]  
    vpsrldq     xmm3,xmm2,2  
    vpinsrw     xmm3,xmm3,word ptr [rax+rcx+0x0F],7  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ebp,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   ecx,xmm2  
    shl         ecx,0x10  
    or          ecx,ebp  
    not         ecx  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   esi,xmm2  
    vpmaxub     xmm2,xmm0,xmm3  
    vpcmpeqb    xmm2,xmm0,xmm2  
    vpmovmskb   r8d,xmm2  
    shl         r8d,0x10  
    or          r8d,esi  
    not         r8d  
    and         edx,edi  
    and         edx,r14d  
    and         ebx,edx  
    or          ebx,ecx  
    and         ebx,r12d  
    and         edx,r9d  
    or          edx,r8d  
    and         edx,ebx  
    je          loop  
    lea         rsi,[rax-3]  
    mov         rbx,qword ptr [rsp+0x48]  
    vmovdqu     xmm2,xmmword ptr [rsi+rbx]  
    mov         rbx,qword ptr [rsp+0x50]  
    vmovdqu     xmm3,xmmword ptr [rsi+rbx]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   ebx,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   esi,xmm2  
    shl         esi,0x10  
    or          esi,ebx  
    not         esi  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm2,xmm1,xmm2  
    vpmovmskb   ebp,xmm2  
    vpminub     xmm2,xmm3,xmm0  
    vpcmpeqb    xmm2,xmm3,xmm2  
    vpmovmskb   ebx,xmm2  
    shl         ebx,0x10  
    or          ebx,ebp  
    not         ebx  
    and         r15d,r11d  
    and         r15d,r8d  
    or          r15d,esi  
    and         r15d,edx  
    mov         r8d,dword ptr [rsp+0x3C]  
    and         r8d,r14d  
    or          r8d,ebx  
    and         r8d,r15d  
    je          loop  
    add         rax,3  
    mov         rdx,qword ptr [rsp+0x48]  
    vmovdqu     xmm2,xmmword ptr [rax+rdx]  
    mov         rdx,qword ptr [rsp+0x50]  
    vmovdqu     xmm3,xmmword ptr [rax+rdx]  
    vpminub     xmm4,xmm1,xmm2  
    vpcmpeqb    xmm4,xmm1,xmm4  
    vpmovmskb   eax,xmm4  
    vpminub     xmm4,xmm2,xmm0  
    vpcmpeqb    xmm2,xmm2,xmm4  
    vpmovmskb   edx,xmm2  
    shl         edx,0x10  
    or          edx,eax  
    not         edx  
    vpminub     xmm2,xmm1,xmm3  
    vpcmpeqb    xmm1,xmm1,xmm2  
    vpmovmskb   eax,xmm1  
    vpminub     xmm0,xmm3,xmm0  
    vpcmpeqb    xmm0,xmm3,xmm0  
    vpmovmskb   ebp,xmm0  
    shl         ebp,0x10  
    or          ebp,eax  
    not         ebp  
    and         esi,dword ptr [rsp+0x38]  
    and         esi,ebx  
    and         ecx,r10d  
    and         ecx,esi  
    or          ecx,edx  
    and         ecx,r8d  
    and         edi,r9d  
    and         edi,esi  
    or          edi,ebp  
    and         edi,ecx  
    je          loop  
    mov         rax,qword ptr [rsp+0x00000000000000A0]  
    mov         qword ptr [rdx],rax  
    add         qword ptr [rcx+8],8
    jmp         loop  

