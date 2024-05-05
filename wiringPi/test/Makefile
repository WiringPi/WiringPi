CC = gcc
CFLAGS = -Wall
LDFLAGS = 

tests = wiringpi_test1_sysfs wiringpi_test2_sysfs wiringpi_test3_device_wpi wiringpi_test4_device_phys wiringpi_test5_default wiringpi_test6_isr wiringpi_test7_version

all: $(tests)

wiringpi_test1_sysfs: 
	${CC} ${CFLAGS} wiringpi_test1_sysfs.c -o wiringpi_test1_sysfs -lwiringPi

wiringpi_test2_sysfs: 
	${CC} ${CFLAGS} wiringpi_test2_sysfs.c -o wiringpi_test2_sysfs -lwiringPi

wiringpi_test3_device_wpi: 
	${CC} ${CFLAGS} wiringpi_test3_device_wpi.c -o wiringpi_test3_device_wpi -lwiringPi

wiringpi_test4_device_phys: 
	${CC} ${CFLAGS} wiringpi_test4_device_phys.c -o wiringpi_test4_device_phys -lwiringPi

wiringpi_test5_default: 
	${CC} ${CFLAGS} wiringpi_test5_default.c -o wiringpi_test5_default -lwiringPi

wiringpi_test6_isr: 
	${CC} ${CFLAGS} wiringpi_test6_isr.c -o wiringpi_test6_isr -lwiringPi

wiringpi_test7_version: 
	${CC} ${CFLAGS} wiringpi_test7_version.c -o wiringpi_test7_version -lwiringPi

test:
	for t in $(tests) ; do \
		echo === unit test: $${t} === ; \
		time ./$${t} ; \
		echo  ; echo  ; \
	done

clean: 
	for t in $(tests) ; do rm -fv $${t} ; done