gcc -W -Wall -o altest altest.c /usr/local/lib/libADTs.a
./altest altest.c 2>&1 | diff - altest.out
rm -f altest
gcc -W -Wall -o bqtest bqtest.c /usr/local/lib/libADTs.a
./bqtest bqtest.c 2>&1 | diff - bqtest.out
rm -f bqtest
gcc -W -Wall -o hmtest hmtest.c /usr/local/lib/libADTs.a
./hmtest hmtest.c 2>&1 | diff - hmtest.out
rm -f hmtest
gcc -W -Wall -o lltest lltest.c /usr/local/lib/libADTs.a
./lltest lltest.c 2>&1 | diff - lltest.out
rm -f lltest
gcc -W -Wall -o ostest ostest.c /usr/local/lib/libADTs.a
./ostest ostest.c 2>&1 | diff - ostest.out
rm -f ostest
gcc -W -Wall -o sttest sttest.c /usr/local/lib/libADTs.a
./sttest sttest.c 2>&1 | diff - sttest.out
rm -f sttest
gcc -W -Wall -o ustest ustest.c /usr/local/lib/libADTs.a
./ustest ustest.c 2>&1 | diff - ustest.out
rm -f ustest
gcc -W -Wall -o uqtest uqtest.c /usr/local/lib/libADTs.a
./uqtest uqtest.c 2>&1 | diff - uqtest.out
rm -f uqtest
gcc -W -Wall -o tsaltest tsaltest.c /usr/local/lib/libADTs.a
./tsaltest tsaltest.c 2>&1 | diff - tsaltest.out
rm -f tsaltest
gcc -W -Wall -o tsbqtest tsbqtest.c /usr/local/lib/libADTs.a
./tsbqtest tsbqtest.c 2>&1 | diff - tsbqtest.out
rm -f tsbqtest
gcc -W -Wall -o tshmtest tshmtest.c /usr/local/lib/libADTs.a
./tshmtest tshmtest.c 2>&1 | diff - tshmtest.out
rm -f tshmtest
gcc -W -Wall -o tslltest tslltest.c /usr/local/lib/libADTs.a
./tslltest tslltest.c 2>&1 | diff - tslltest.out
rm -f tslltest
gcc -W -Wall -o tsostest tsostest.c /usr/local/lib/libADTs.a
./tsostest tsostest.c 2>&1 | diff - tsostest.out
rm -f tsostest
gcc -W -Wall -o tssttest tssttest.c /usr/local/lib/libADTs.a
./tssttest tssttest.c 2>&1 | diff - tssttest.out
rm -f tssttest
gcc -W -Wall -o tsustest tsustest.c /usr/local/lib/libADTs.a
./tsustest tsustest.c 2>&1 | diff - tsustest.out
rm -f tsustest
gcc -W -Wall -o tsuqtest tsuqtest.c /usr/local/lib/libADTs.a
./tsuqtest tsuqtest.c 2>&1 | diff - tsuqtest.out
rm -f tsuqtest
