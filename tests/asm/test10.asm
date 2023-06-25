.x64
.model flat

.code
lea rax, [test_data]

.data
org 200h
test_data db 123

end
