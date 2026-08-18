rm -rf a.out
rm -rf ./bin/*.bin
gcc header_to_bin.c -I$1 -I$2 -I$3 -DCUSTOM_TUNING=$4
./a.out
rm -rf a.out 

chmod 777 -R ./bin/*.bin


