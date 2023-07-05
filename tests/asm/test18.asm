.x64
.model flat

.code
main:
sub     rsp, 40
mov     r9d, 4
mov     r8d, 3
mov     edx, 2
mov     ecx, 1
call    f2
xor     eax, eax
add     rsp, 40
ret

f2:
mov     DWORD PTR [rsp+32], r9d
mov     DWORD PTR [rsp+24], r8d
mov     DWORD PTR [rsp+16], edx
mov     DWORD PTR [rsp+8], ecx
ret


end