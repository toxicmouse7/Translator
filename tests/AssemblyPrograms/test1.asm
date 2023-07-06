.x64

.code

LoadLibraryA proto :qword
GetProcAddress proto :qword, :qword
GetLastError proto

main proc

    sub rsp, 40

    lea rcx, [user32_lib]
    call LoadLibraryA

    mov [user32_ptr], rax

    xor rax, rax
    add rsp, 40
    ret

main endp


.data

user32_lib db "user32.dll", 0
kernerl32_lib db "kernel32.dll", 0
messageBox_func db "MessageBoxA", 0

user32_ptr dq 0



end