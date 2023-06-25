.x64
.model flat

.code
mov qword ptr [test_data], 0ffffffffeeeeeeeeh
mov qword ptr [test_data], 7fffffffh

.data
org 200h
test_data dq 0

end
