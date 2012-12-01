echo "========= BUILDING GETART ==========\n\n"

clang getart.c -ggdb
mv a.out getart
chmod +x getart

echo "========= BUILDING BMP2SCDASIC ==========\n\n"

clang bmp2scdasic.c -ggdb
mv a.out bmp2scdasic
chmod +x bmp2scdasic

echo "========== BUILDING SINE GENERATOR ==========\n\n"

clang sinegen.c -ggdb
mv a.out sinegen
chmod +x sinegen

echo "========== BUILDING PCM BANK MAKER ==========\n\n"
clang pcmbankmaker.c -ggdb
mv a.out pcmbankmaker
chmod +x pcmbankmaker

echo "========== BUILDING VDP CALCULATOR ==========\n\n"
clang vdpcalc.c -ggdb
mv a.out vdpcalc
chmod +x vdpcalc
