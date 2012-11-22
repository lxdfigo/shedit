#Makefile of the shedit

objects = edit.o ui.o controller.o menufuncs.o utility.o
CC = cc

shedit : $(objects)
	$(CC) -o shedit $(objects) shedit.c -lcurses

utility.o : edit.h basic.h

menufuncs.o : edit.h basic.h

controller.o : controller.h basic.h

ui.o : ui.h controller.h basic.h

edit.o : edit.h controller.h basic.h

clean:
	rm shedit $(objects)
	
install:
	mv ./shedit /bin/
	mv ./shedit.config /etc/
