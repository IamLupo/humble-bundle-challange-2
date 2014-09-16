@echo off

g++ main.cpp -o main.exe -static

pause

::set project=demo
::main.exe -e "%project%/input.txt" "%project%/input_key.txt" "%project%/output.txt" "%project%/output_key.txt"

set project=crackme
main.exe -e "%project%/crackme.zip" "%project%/input_key.txt" "%project%/crackme.zip.enc" "%project%/output_key.txt"
::main.exe -d "%project%/crackme.zip.enc" "%project%/input_key.txt" "%project%/crackme.zip" "%project%/output_key.txt"

pause