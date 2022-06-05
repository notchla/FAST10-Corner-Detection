.intel_syntax noprefix

loop:
    mov         rax,qword ptr [rsp+0x00000000000000F8]  
    mov         rcx,qword ptr [rsp+0x28]  
    add         rcx,rax  
    mov         rdx,qword ptr [rsp+0x00000000000000F0]  
    lea         r11,[rdx+rcx]  
    vmovdqu64   zmm1,zmmword ptr [rdx+rcx]  
    vpsubusb    zmm0,zmm1,zmm5  
    vpaddusb    zmm1,zmm5,zmm1  
    mov         rcx,qword ptr [rsp+0x00000000000000B0]  
    vmovdqu64   zmm2,zmmword ptr [rcx+r11]  
    mov         rcx,qword ptr [rsp+0x00000000000000A8]  
    vmovdqu64   zmm3,zmmword ptr [rcx+r11]  
    vpcmpub     k0,zmm0,zmm2,2  
    kmovq       r8,k0  
    vpcmpub     k0,zmm2,zmm1,2  
    kmovq       r10,k0  
    vmovq       xmm2,r10  
    vmovq       xmm4,r8  
    vpunpcklqdq xmm2,xmm4,xmm2  
    vpcmpub     k0,zmm0,zmm3,2  
    kmovq       r9,k0  
    vpcmpub     k0,zmm3,zmm1,2  
    kmovq       r12,k0  
    vmovq       xmm3,r12  
    vmovq       xmm4,r9  
    vpunpcklqdq xmm3,xmm4,xmm3  
    vpternlogq  xmm3,xmm3,xmm3,0x0F  
    vpternlogq  xmm3,xmm2,xmm6,0x0F6  
    vptest      xmm3,xmm3  
    je          loop  
    not         r10  
    not         r8  
    not         r12  
    not         r9  
    mov         rax,qword ptr [rsp+0x0000000000000098]  
    vmovdqu64   zmm2,zmmword ptr [r11+rax+0xFFFFFFFFFFFFFFFE]  
    mov         rcx,qword ptr [rsp+0x00000000000000A0]  
    vmovdqu64   zmm3,zmmword ptr [r11+rcx+0x0000000000000002]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    kmovq       rdi,k1  
    kmovq       rsi,k0  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    kmovq       r15,k1  
    kmovq       rax,k0  
    mov         qword ptr [rsp+0x0000000000000100],rsi  
    and         rsi,r8  
    mov         qword ptr [rsp+0x38],rdi  
    and         rdi,r10  
    mov         r14,rax  
    and         r14,r9  
    mov         qword ptr [rsp+0x0000000000000090],r15  
    and         r15,r12  
    or          rdi,r15  
    or          rsi,r14  
    mov         rdx,rsi  
    or          rdx,rdi  
    je          loop  
    lea         rdx,[r11-2]  
    lea         rbp,[r11+2]  
    mov         qword ptr [rsp+0x78],rax  
    mov         rax,qword ptr [rsp+0x00000000000000A0]  
    vmovdqu64   zmm2,zmmword ptr [rdx+rax]  
    mov         rax,qword ptr [rsp+0x0000000000000098]  
    vmovdqu64   zmm3,zmmword ptr [rbp+rax]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    kmovq       rbp,k1  
    kmovq       rax,k0  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    kmovq       rcx,k0  
    kmovq       rbx,k1  
    and         r9,rax  
    mov         rdx,r9  
    or          rdx,rcx  
    and         rdx,rsi  
    mov         qword ptr [rsp+0x0000000000000088],rcx  
    mov         rsi,rcx  
    and         rsi,r8  
    mov         qword ptr [rsp+0x68],rax  
    or          rsi,rax  
    and         r12,rbp  
    and         rdx,rsi  
    mov         rsi,r12  
    or          rsi,rbx  
    and         rsi,rdi  
    mov         qword ptr [rsp+0x0000000000000080],rbx  
    mov         rdi,rbx  
    and         rdi,r10  
    mov         qword ptr [rsp+0x70],rbp  
    or          rdi,rbp  
    and         rsi,rdi  
    mov         rdi,rdx  
    or          rdi,rsi  
    je          loop  
    vmovdqu64   zmm2,zmmword ptr [r11+0xFFFFFFFFFFFFFFFD]  
    vmovdqu64   zmm3,zmmword ptr [r11+0x0000000000000003]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    kmovq       rax,k1  
    mov         qword ptr [rsp+0x60],rax  
    kmovq       rbx,k0  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    kmovq       r13,k1  
    kmovq       rcx,k0  
    mov         rbp,rcx  
    and         rbp,qword ptr [rsp+0x0000000000000088]  
    mov         rax,r13  
    and         rax,qword ptr [rsp+0x0000000000000080]  
    mov         rdi,qword ptr [rsp+0x68]  
    and         rdi,qword ptr [rsp+0x0000000000000100]  
    and         rdi,rbx  
    mov         qword ptr [rsp+0x00000000000000C8],rcx  
    or          rdi,rcx  
    and         rdi,rdx  
    mov         qword ptr [rsp+0x00000000000000E0],rbp  
    mov         rdx,rbp  
    and         rdx,qword ptr [rsp+0x78]  
    mov         qword ptr [rsp+0x00000000000000C0],rbx  
    or          rdx,rbx  
    and         rdi,rdx  
    mov         rdx,qword ptr [rsp+0x70]  
    and         rdx,qword ptr [rsp+0x38]  
    mov         rcx,qword ptr [rsp+0x60]  
    and         rdx,rcx  
    mov         qword ptr [rsp+0x00000000000000D0],r13  
    or          rdx,r13  
    and         rdx,rsi  
    mov         qword ptr [rsp+0x00000000000000D8],rax  
    mov         rsi,rax  
    and         rsi,qword ptr [rsp+0x0000000000000090]  
    or          rsi,rcx  
    and         rdx,rsi  
    mov         rsi,rdi  
    or          rsi,rdx  
    je          loop  
    mov         rsi,qword ptr [rsp+0x00000000000000B0]  
    vmovdqu64   zmm2,zmmword ptr [r11+rsi+0xFFFFFFFFFFFFFFFF]  
    vmovdqu64   zmm3,zmmword ptr [r11+rsi+0x0000000000000001]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    kmovq       r13,k0  
    kmovq       rax,k1  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    kmovq       rsi,k1  
    kmovq       rcx,k0  
    or          r15,rax  
    or          r14,r13  
    and         r14,rdi  
    mov         rdi,rcx  
    and         r15,rdx  
    or          r9,rcx  
    and         r9,r14  
    or          r12,rsi  
    and         r12,r15  
    mov         rcx,r9  
    or          rcx,r12  
    je          loop  
    mov         r14,rax  
    lea         rcx,[r11-1]  
    mov         rdx,qword ptr [rsp+0x00000000000000A8]  
    vmovdqu64   zmm2,zmmword ptr [rcx+rdx]  
    lea         rcx,[r11+1]  
    vmovdqu64   zmm3,zmmword ptr [rcx+rdx]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    kmovq       r15,k1  
    and         r10,rax  
    and         r10,rsi  
    mov         rbx,rsi  
    mov         rsi,qword ptr [rsp+0x0000000000000080]  
    and         rsi,r10  
    or          rsi,r15  
    and         rsi,r12  
    kmovq       rcx,k0  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    and         r8,r13  
    and         r8,rdi  
    mov         rax,qword ptr [rsp+0x0000000000000088]  
    and         rax,r8  
    or          rax,rcx  
    and         rax,r9  
    kmovq       rdx,k1  
    and         r10,qword ptr [rsp+0x38]  
    or          r10,rdx  
    and         r10,rsi  
    mov         rsi,rbx  
    kmovq       rbx,k0  
    and         r8,qword ptr [rsp+0x0000000000000100]  
    or          r8,rbx  
    and         r8,rax  
    mov         rbp,r8  
    or          rbp,r10  
    je          loop  
    lea         rbp,[r11-3]  
    mov         rax,qword ptr [rsp+0x50]  
    vmovdqu64   zmm2,zmmword ptr [rbp+rax]  
    mov         rax,qword ptr [rsp+0x58]  
    vmovdqu64   zmm3,zmmword ptr [rbp+rax]  
    mov         rbp,qword ptr [rsp+0x00000000000000D0]  
    and         rbp,qword ptr [rsp+0x0000000000000090]  
    mov         r9,rdi  
    mov         rdi,qword ptr [rsp+0x00000000000000C8]  
    and         rdi,qword ptr [rsp+0x78]  
    and         rdi,rbx  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    and         rbp,rdx  
    kmovq       rdx,k0  
    or          rdi,rdx  
    and         rdi,r8  
    kmovq       rbx,k1  
    or          rbp,rbx  
    and         rbp,r10  
    mov         rax,qword ptr [rsp+0x00000000000000D8]  
    and         rax,rsi  
    vpcmpub     k0,zmm0,zmm3,6  
    vpcmpub     k1,zmm3,zmm1,6  
    mov         rsi,qword ptr [rsp+0x00000000000000E0]  
    and         rsi,r9  
    kmovq       r10,k0  
    or          rsi,r10  
    and         rsi,rdi  
    kmovq       rdi,k1  
    or          rax,rdi  
    and         rax,rbp  
    mov         r8,rsi  
    or          rsi,rax  
    je          loop  
    mov         r9,rax  
    add         r11,3  
    mov         rax,qword ptr [rsp+0x50]  
    vmovdqu64   zmm2,zmmword ptr [r11+rax]  
    mov         rax,qword ptr [rsp+0x58]  
    vmovdqu64   zmm3,zmmword ptr [r11+rax]  
    and         rbx,qword ptr [rsp+0x60]  
    and         rdx,qword ptr [rsp+0x00000000000000C0]  
    and         rdx,r10  
    and         rbx,rdi  
    and         rcx,qword ptr [rsp+0x68]  
    vpcmpub     k0,zmm0,zmm2,6  
    vpcmpub     k1,zmm2,zmm1,6  
    and         r15,qword ptr [rsp+0x70]  
    kmovq       rax,k0  
    and         rcx,rdx  
    or          rcx,rax  
    kmovq       rax,k1  
    and         r15,rbx  
    or          r15,rax  
    and         r15,r9  
    and         rcx,r8  
    and         r14,qword ptr [rsp+0x38]  
    and         r13,qword ptr [rsp+0x0000000000000100]  
    and         r13,rdx  
    vpcmpub     k0,zmm3,zmm1,6  
    and         r14,rbx  
    kmovq       rax,k0  
    or          r14,rax  
    vpcmpub     k0,zmm0,zmm3,6  
    and         r14,r15  
    kmovq       rax,k0  
    or          r13,rax  
    and         r13,rcx  
    or          r13,r14  
    je          loop  
    popcnt      rsi,r13  
    lea         r8,[rsi*8+0x0000000000000000]  
    mov         rcx,rbp  
    xor         edx,edx  
    vmovdqu64   zmmword ptr [rsp+0x100],zmm18  
    vmovdqu64   zmm18,zmmword ptr [rsp+0x100]  
    vmovdqu64   zmm17,zmmword ptr [rsp+0x140]  
    vmovdqa64   zmm16,zmmword ptr [rsp+0x200]  
    vmovdqu64   zmm5,zmmword ptr [rsp+0x180]  
    lea         rbp,[rbp+rsi*8]  
    mov         qword ptr [rbx+8],rbp  
    shl         rdi,3  
    add         rdi,qword ptr [rbx]  
    mov         rax,qword ptr [rsp+0x28]  
    vpbroadcastd zmm0,eax  
    vporq       zmm0,zmm0,zmm16  
    mov         rax,0x0FFFFFFFFFFFFFFF8  
    nop  
loop_compress:
    mov         cx,0x0AAAA  
    kmovd       k1,ecx  
    vpblendmd   zmm1{k1},zmm0,zmm18  
    movzx       ecx,r13b  
    kmovd       k1,r13d  
    vpcompressq zmmword ptr [rdi]{k1},zmm1  
    popcnt      ecx,ecx  
    lea         rdi,[rdi+rcx*8]  
    shrd        r13,r14,8  
    vpaddd      zmm0,zmm0,zmm17  
    shr         r14,8  
    add         rax,8  
    cmp         rax,0x38  
    jb          loop_compress 
    jmp         loop

                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
