echo "========= BUILDING GETART ==========\n\n"

clang getart.c
mv a.out getart
chmod +x getart

echo "========= BUILDING BMP2SCDASIC ==========\n\n"

clang bmp2scdasic.c
mv a.out bmp2scdasic
chmod +x bmp2scdasic

echo "========== BUILDING SINE GENERATOR ==========\n\n"

clang sinegen.c
mv a.out sinegen
chmod +x sinegen
