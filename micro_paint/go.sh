gcc micro.c -o my

./my $1 > res1;
./our $1 > res2;
echo --------------------------
cat res1
echo --------------------------
cat res2
echo -----------diff-----------
diff res1 res2
echo -----------diff-----------

