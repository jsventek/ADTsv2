t='test'
for f in al bq hm ll os st us uq tsal tsbq tshm tsll tsos tsst tsus tsuq; do
    echo Testing $f$t >/dev/tty
    gcc -W -Wall -o $f$t $f$t.c /usr/local/lib/libADTs.a -lpthread
    ./$f$t $f$t.c 2>&1 | diff - $f$t.out
    rm -f $f$t
done
