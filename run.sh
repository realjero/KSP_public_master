gcc -g -Wall -std=c99 -pedantic -o vm njvm.c \
&& \
printf '\nCompiled succesfully\n\n\n\n' \
&& \
./vm bin/code1.bin