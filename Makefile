#Makefile of the shedit

objects = edit.o ui.o controller.o
CC = cc

shedit : $(objects)
	$(CC) -o shedit $(objects) shedit.c -lcurses

controller.o : controller.h

ui.o : ui.h controller.h

edit.o : edit.h controller.h

clean:
	rm shedit $(objects)
	
install:
	mv ./shedit /bin/
	mv ./shedit.config /etc/
