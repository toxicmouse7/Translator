../venv/Scripts/activate

$test_count = 12

for ($i = 1; $i -le $test_count; $i++)
{
    cd asm && jwasm -nologo -bin test$($i).asm && cd .. && python generate_cpp_test.py test$($i)

    if ($args[0] -eq '--clean')
    {
        rm asm/test$($i).bin
    }
}

python generate_cpp_test_include.py $test_count
