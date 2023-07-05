.x64
.model flat

.code
add rax, [test_var]

org 200h
test_var dq 0

end
