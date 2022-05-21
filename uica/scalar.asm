.intel_syntax noprefix

loop: 
    mov         rax,qword ptr [rsp+0x0000000000000090]  
    movzx       eax,byte ptr [rax+r12+3]  
    mov         ecx,eax  
    mov         ebp,dword ptr [rsp+0x4C]  
    sub         ecx,ebp  
    mov         edx,ecx  
    sar         edx,0x1F  
    andn        r9d,edx,ecx  
    add         eax,ebp  
    cmp         eax,0x0FF  
    mov         ecx,0x0FF  
    cmovae      eax,ecx  
    mov         rcx,qword ptr [rsp+0x0000000000000088]  
    movzx       edx,byte ptr [rcx+r12-1]  
    mov         rcx,qword ptr [rsp+0x0000000000000080]  
    movzx       ebp,byte ptr [rcx+r12-1]  
    xor         ebx,ebx  
    cmp         r9d,edx  
    seta        bl  
    xor         edi,edi  
    cmp         eax,edx  
    setb        dil  
    lea         r15d,[rbx+rdi*2]  
    xor         edx,edx  
    cmp         r9d,ebp  
    seta        dl  
    xor         ebx,ebx  
    cmp         eax,ebp  
    setb        bl  
    lea         r8d,[rdx+rbx*2]  
    mov         edx,r15d  
    or          edx,r8d
    je          loop
    mov         rcx,qword ptr [rsp+0x70]  
    movzx       edx,byte ptr [rcx+r12-4]  
    xor         ebx,ebx  
    cmp         r9d,edx  
    seta        bl  
    mov         rcx,qword ptr [rsp+0x78]  
    movzx       edi,byte ptr [rcx+r12]  
    xor         esi,esi  
    cmp         eax,edx  
    setb        sil  
    xor         edx,edx  
    cmp         r9d,edi  
    seta        dl  
    lea         ecx,[rbx+rsi*2]  
    xor         ebx,ebx  
    cmp         eax,edi  
    setb        bl  
    lea         r10d,[rdx+rbx*2]  
    mov         dword ptr [rsp+0x2C],ecx  
    mov         edx,ecx  
    and         edx,r15d  
    mov         ebx,r10d  
    and         ebx,r8d  
    or          edx,ebx  
    je          loop
    mov         rcx,qword ptr [rsp+0x78]  
    movzx       edi,byte ptr [rcx+r12-4]  
    mov         rcx,qword ptr [rsp+0x70]  
    movzx       esi,byte ptr [rcx+r12]  
    xor         ecx,ecx  
    cmp         r9d,edi  
    seta        cl  
    xor         ebp,ebp  
    cmp         eax,edi  
    setb        bpl  
    lea         ecx,[rcx+rbp*2]  
    xor         ebp,ebp  
    cmp         r9d,esi  
    seta        bpl  
    xor         edi,edi  
    cmp         eax,esi  
    setb        dil  
    lea         r13d,[rbp+rdi*2]  
    mov         ebp,r13d  
    and         ebp,r15d  
    or          ebp,ecx  
    mov         dword ptr [rsp+0x34],ecx  
    and         r8d,ecx  
    mov         edi,r13d  
    or          edi,r8d  
    and         edi,edx  
    and         edi,ebp  
    je          loop
    mov         rcx,qword ptr [rsp+0x0000000000000090]  
    movzx       edx,byte ptr [rcx+r12]  
    movzx       ebp,byte ptr [rcx+r12+6]  
    xor         esi,esi  
    cmp         r9d,edx  
    seta        sil  
    xor         ecx,ecx  
    cmp         eax,edx  
    setb        cl  
    lea         esi,[rsi+rcx*2]  
    xor         ecx,ecx  
    cmp         r9d,ebp  
    seta        cl  
    xor         edx,edx  
    cmp         eax,ebp  
    setb        dl  
    lea         edx,[rcx+rdx*2]  
    mov         r14d,edx  
    and         r14d,r13d  
    mov         ecx,r14d  
    and         ecx,r10d  
    or          ecx,esi  
    mov         r11d,dword ptr [rsp+0x34]  
    and         r11d,dword ptr [rsp+0x2C]  
    mov         dword ptr [rsp+0x48],esi  
    and         r11d,esi  
    or          r11d,edx  
    and         r11d,edi  
    and         r11d,ecx  
    je          loop
    mov         rsi,qword ptr [rsp+0x0000000000000088]  
    movzx       ecx,byte ptr [rsi+r12-2]  
    movzx       esi,byte ptr [rsi+r12]  
    xor         edi,edi  
    cmp         r9d,ecx  
    seta        dil  
    xor         ebp,ebp  
    cmp         eax,ecx  
    setb        bpl  
    lea         ebp,[rdi+rbp*2]  
    xor         ecx,ecx  
    cmp         r9d,esi  
    seta        cl  
    xor         edi,edi  
    cmp         eax,esi  
    setb        dil  
    lea         edi,[rcx+rdi*2]  
    mov         dword ptr [rsp+0x30],ebp  
    or          ebx,ebp  
    and         ebx,r11d  
    or          r8d,edi  
    and         r8d,ebx  
    je          loop
    mov         rbp,qword ptr [rsp+0x0000000000000080]  
    movzx       ecx,byte ptr [rbp+r12-2]  
    movzx       ebp,byte ptr [rbp+r12]  
    xor         ebx,ebx  
    cmp         r9d,ecx  
    seta        bl  
    xor         esi,esi  
    cmp         eax,ecx  
    setb        sil  
    lea         esi,[rbx+rsi*2]  
    xor         ecx,ecx  
    cmp         r9d,ebp  
    seta        cl  
    xor         ebx,ebx  
    cmp         eax,ebp  
    setb        bl  
    lea         r11d,[rcx+rbx*2]  
    and         r15d,dword ptr [rsp+0x30]  
    and         r15d,edi  
    and         r13d,r15d  
    mov         dword ptr [rsp+0x44],esi  
    or          r13d,esi  
    and         r13d,r8d  
    and         r15d,dword ptr [rsp+0x2C]  
    or          r15d,r11d  
    and         r15d,r13d  
    je          loop
    mov         rcx,qword ptr [rsp+0x60]  
    movzx       ecx,byte ptr [rcx+r12]  
    xor         ebp,ebp  
    cmp         r9d,ecx  
    seta        bpl  
    mov         rbx,qword ptr [rsp+0x68]  
    movzx       esi,byte ptr [rbx+r12]  
    xor         ebx,ebx  
    cmp         eax,ecx  
    setb        bl  
    xor         ecx,ecx  
    cmp         r9d,esi  
    seta        cl  
    lea         ebp,[rbp+rbx*2]  
    xor         ebx,ebx  
    cmp         eax,esi  
    setb        bl  
    lea         r8d,[rcx+rbx*2]  
    and         edx,r10d  
    and         edx,r11d  
    or          edx,ebp  
    and         edx,r15d  
    and         r14d,edi  
    or          r14d,r8d  
    and         r14d,edx  
    je          loop
    mov         rcx,qword ptr [rsp+0x60]  
    movzx       edi,byte ptr [rcx+r12+6]  
    mov         rcx,qword ptr [rsp+0x68]  
    movzx       esi,byte ptr [rcx+r12+6]  
    xor         ebx,ebx  
    cmp         r9d,edi  
    seta        bl  
    xor         edx,edx  
    cmp         eax,edi  
    setb        dl  
    add         edx,edx  
    xor         edi,edi  
    cmp         r9d,esi  
    seta        dil  
    xor         ecx,ecx  
    cmp         eax,esi  
    setb        cl  
    add         ecx,ecx  
    or          ecx,edi  
    and         ebp,dword ptr [rsp+0x48]  
    and         ebp,r8d  
    mov         eax,dword ptr [rsp+0x44]  
    and         eax,dword ptr [rsp+0x34]  
    or          edx,ebx  
    and         eax,ebp  
    or          edx,eax  
    and         edx,r14d  
    mov         eax,dword ptr [rsp+0x30]  
    and         eax,dword ptr [rsp+0x2C]  
    and         eax,ebp  
    or          eax,ecx  
    test        edx,eax  
    je          loop
    lea         eax,[r12+3]  
    mov         dword ptr [rsp+0x00000000000000D8],eax  
    mov         rax,qword ptr [rsp+0x38]  
    mov         dword ptr [rsp+0x00000000000000DC],eax  
    mov         rcx,qword ptr [rsp+0x0000000000000150]  
    mov         rdx,qword ptr [rcx+8]  
    cmp         rdx,qword ptr [rcx+0x10]  
    jz          loop
    mov         rax,qword ptr [rsp+0x00000000000000D8]  
    mov         qword ptr [rdx],rax  
    add         qword ptr [rcx+8],8  
    jmp         loop

