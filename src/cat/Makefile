all: s21_cat clean


cat.o: cat.c
	gcc -c -Wall -Wextra -Werror -std=c11 cat.c

s21_cat: cat.o
	gcc -Wall -Wextra -Werror -std=c11 cat.o -o s21_cat


clean:
	rm -rf *.o
