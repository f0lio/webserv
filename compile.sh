

clear;make re 2>logs.txt; [ -s logs.txt ] && echo "yes error" || echo "no error"