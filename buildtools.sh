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


echo "========== BUILDING MOD2PCM CALCULATOR ==========\n\n"
clang mod2pcm/mod2pcm.c -ggdb
mv a.out mod2pcm/mod2pcm
chmod +x mod2pcm/mod2pcm

echo "========== BUILDING TWO'S COMPLIMENT TO SIGN/MAG CONVERTER ==========\n\n"
clang sign2signmag.c -ggdb
mv a.out sign2signmag
chmod +x sign2signmag


#./mod2pcm/mod2pcm mod2pcm/stardstm.mod
