CC = gcc
CFLAGS = -Wall
LDFLAGS = 

tests = wiringpi_test1_sysfs wiringpi_test2_sysfs wiringpi_test3_device_wpi wiringpi_test4_device_phys wiringpi_test5_default wiringpi_test6_isr wiringpi_test7_version

xotests = wiringpi_xotest_test1_spi

all: $(tests) $(xotests)

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

wiringpi_xotest_test1_spi:
	${CC} ${CFLAGS} wiringpi_xotest_test1_spi.c -o wiringpi_xotest_test1_spi -lwiringPi

test:
	@error_state=false ; \
	for t in $(tests) ; do \
		echo === unit test: $${t} === ; \
		time ./$${t}; \
		if [ $$? -ne 0 ]; then \
		  error_state=true ; \
		fi ; \
		echo  ; echo  ; \
	done ; \
	if [ "$$error_state" = true ]; then \
		echo "STD TEST FAILED"; \
	else \
		echo "STD TEST SUCCESS"; \
	fi

xotest:
	@error_state=false ; \
	for t in $(tests) $(xotests)  ; do \
		echo === unit test: $${t} === ; \
		time ./$${t} ; \
		if [ $$? -ne 0 ]; then \
		  error_state=true ; \
		fi ; \
		echo  ; echo  ; \
	done
	if [ "$$error_state" = true ]; then \
		echo "XO TEST FAILED"; \
	else \
		echo "XO TEST SUCCESS"; \
	fi

clean: 
	for t in $(tests) $(xotests) ; do \
		rm -fv $${t} ; \
	done
