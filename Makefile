obj-m += miniproject_main.o
BUILDROOTDIR := /home/student/Desktop/alb/buildroot
KERNELDIR := $(BUILDROOTDIR)/output/build/linux-2.6.35.4
CROSS := $(BUILDROOTDIR)/output/staging/usr/bin/avr32-linux-gcc

all: miniproject_main.o udp.o
	$(CROSS) -pthread miniproject_main.o udp.o -o miniproject
	cp miniproject /export/nfs/miniproject

miniproject_main.o: miniproject_main.c
	$(CROSS) -pthread -c miniproject_main.c -o miniproject_main.o

udp.o: network/udp.c
	$(CROSS) -c network/udp.c -o udp.o

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
