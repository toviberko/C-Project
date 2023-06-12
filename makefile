SOURCES = main.c first_pass.c second_pass.c general_functions.c spread_macros.c
HEADERS = header.h

OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
CC = gcc
CFLAGS = -Wall -pedantic -ansi -g

main: $(OBJECTS) $(HEADERS)
	$(CC) -g $(CFLAGS) $(OBJECTS) -o $@

general_functions.o: general_functions.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


second_pass.o: second_pass.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

first_pass.o: first_pass.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

spread_macros.o: spread_macros.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJECTS) main
