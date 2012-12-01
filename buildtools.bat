@echo off

echo ========= BUILDING GETART ==========

gcc getart.c --std=c99
move a.exe getart.exe

echo ========= BUILDING BMP2SCDASIC ==========

gcc bmp2scdasic.c --std=c99
move a.exe bmp2scdasic.exe

echo ========== BUILDING SINE GENERATOR ==========

gcc sinegen.c --std=c99
move a.exe sinegen.exe

echo ========== BUILDING PCM BANK MAKER ==========
gcc pcmbankmaker.c --std=c99
move a.exe pcmbankmaker.exe

echo ========== BUILDING VDP CALCULATOR ==========
gcc vdpcalc.c --std=c99
move a.exe vdpcalc.exe
vdpcalc.exe