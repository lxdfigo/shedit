#Makefile of the shedit

objects = edit.o ui.o controller.o
CC = cc

shedit : $(objects)
	$(CC) -o shedit $(objects) shedit.c -lcurses

controller.o : controller.h basic.h

ui.o : ui.h controller.h basic.h

edit.o : edit.h controller.h basic.h

clean:
	rm shedit $(objects)
	
install:
	mv ./shedit /bin/
	mv ./shedit.config /etc/
