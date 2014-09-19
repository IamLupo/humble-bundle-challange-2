@echo off

g++ main.cpp -o main.exe -static

pause

set project=crackme
::Encrypt
::main.exe -e "%project%/crackme.zip" "%project%/input_key.txt" "%project%/crackme.zip.enc" "%project%/output_key.txt"
::Decrypt
main.exe -d "%project%/crackme.zip.enc" "%project%/output_key.txt" "%project%/dec_crackme.zip" "%project%/dec_output_key.txt"

pause