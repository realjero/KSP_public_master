for t in asm/*.asm; do
  file="${t%.*}"
  fileoexten="${file##*/}"
  ./nja $t bin/$fileoexten.bin
  rm
done;
