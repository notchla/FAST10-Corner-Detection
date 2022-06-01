.intel_syntax noprefix

loop: 
    add         r13,0x20  
    cmp         r13,qword ptr [rsp+0x0000000000000088]  
    jge         loop  
    mov         rax,qword ptr [rsp+0x0000000000000098]  
    lea         rcx,[rax+r13]  
    mov         rdx,qword ptr [rsp+0x0000000000000090]  
    lea         rax,[rdx+rcx]  
    vmovdqu     ymm1,ymmword ptr [rdx+rcx]  
    vpsubusb    ymm0,ymm1,ymm4  
    vpaddusb    ymm1,ymm4,ymm1  
    mov         rcx,qword ptr [rsp+0x68]  
    vmovdqu     ymm2,ymmword ptr [rcx+rax]  
    mov         rcx,qword ptr [rsp+0x60]  
    vmovdqu     ymm3,ymmword ptr [rcx+rax]  
    vpcmpub     k0,ymm0,ymm2,2  
    vpcmpub     k1,ymm2,ymm1,2  
    kmovd       ecx,k0  
    kmovd       edx,k1  
    shl         rdx,0x20  
    or          rdx,rcx  
    not         rdx  
    vpcmpub     k0,ymm0,ymm3,2  
    vpcmpub     k1,ymm3,ymm1,2  
    kmovd       ecx,k0  
    kmovd       ebp,k1  
    shl         rbp,0x20  
    or          rbp,rcx  
    not         rbp  
    mov         rcx,rbp  
    or          rcx,rdx  
    je          loop 