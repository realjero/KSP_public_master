gcc -g -Wall -std=c99 -pedantic -o vm njvm.c;
for t in testfiles/bin/*.bin; do
  echo "TESTING: $t";
  echo "NJVM:";
  ./vm $t > o1;
  echo "REF:";
  ./testfiles/njvm $t > o2;
  DIFF=$(diff o1 o2);
  if [ "$DIFF" == "" ]; then
    echo "PASSED";
  fi
done;
rm o1;
rm o2;
rm vm