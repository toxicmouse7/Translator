../venv/Scripts/activate

$test_count = 5

for ($i = 1; $i -le $test_count; $i++)
{
    if ($args[0] -eq '--clean')
    {
        rm asm/test$($i).bin
    }

    jwasm -nologo -bin asm/test$($i).asm && mv test$($i).BIN asm/test$($i).bin && python generate_cpp_test.py test$($i)

    if ($args[0] -eq '--clean')
    {
        rm asm/test$($i).bin
    }
}

python generate_cpp_test_include.py $test_count
