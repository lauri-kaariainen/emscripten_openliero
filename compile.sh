cd liero;
make;
cp openliero ../openliero.bc;
cd ..;
emcc openliero.bc -s TOTAL_MEMORY=36777216  -o openliero.html --preload-file LEVEDIT.EXE --preload-file LICENSE.TXT --preload-file LIERO.CHR --preload-file LIERO.DAT --preload-file LIEROENG.TXT --preload-file LIERO.EXE --preload-file LIERO.OPT --preload-file LIERO.SND --preload-file LIERO.TXT --preload-file NAMES.DAT --preload-file WINXP.BAT --preload-file FILE_ID.DIZ --preload-file LIERO.dat --preload-file Profiles/ --preload-file STBANA2.LEV --preload-file  STBANA2.bmp

