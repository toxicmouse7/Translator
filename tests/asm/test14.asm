.x64
.model flat

.code
add [test_var], 012341234h

org 200h
test_var dq 0

end
