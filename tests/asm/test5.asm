.x64
.model flat

.code
mov qword ptr [test_data], rax
; push eax
; mov eax, dword ptr [highEax]
; mov dword ptr [rip + test_data + 4], eax
; pop eax
; mov dword ptr [rip + test_data], eax

.data
org 200h
test_data dq 0

end
