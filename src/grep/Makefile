all: s21_grep clean


s21_grep.o: s21_grep.c
	gcc -D_GNU_SOURCE -c -Wall -Wextra -Werror -std=c11 s21_grep.c

s21_grep: s21_grep.o
	gcc -D_GNU_SOURCE -Wall -Wextra -Werror -std=c11 s21_grep.o -o s21_grep


clean:
	rm -rf *.o
