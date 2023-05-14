.x64
.model flat

.code
mov rax, qword ptr [test_data]
; mov eax, dword ptr [rip + test_data + 4]
; mov [highEax], eax
; mov eax, dword ptr [rip + test_data]

.data
org 200h
test_data dq 0

end
