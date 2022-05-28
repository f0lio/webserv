clear;make re 2>logs.txt; [ -s logs.txt ] && echo "Error: check logs.txt" || make run

