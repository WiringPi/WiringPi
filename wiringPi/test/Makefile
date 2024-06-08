CC = gcc
CFLAGS = -Wall
LDFLAGS = 

# Need BCM26 <-> BCM19 connected (1kOhm)
tests = wiringpi_test1_sysfs wiringpi_test2_sysfs wiringpi_test3_device_wpi wiringpi_test4_device_phys wiringpi_test5_default wiringpi_test6_isr wiringpi_test7_version

# Need XO hardware
xotests = wiringpi_xotest_test1_spi wiringpi_i2c_test1_pcf8574

# Need PiFace hardware and tests condition
pifacetests = wiringpi_piface_test1

all: $(tests) $(xotests) $(pifacetests)

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

wiringpi_i2c_test1_pcf8574:
	${CC} ${CFLAGS} wiringpi_i2c_test1_pcf8574.c -o wiringpi_i2c_test1_pcf8574 -lwiringPi

wiringpi_piface_test1:
	${CC} ${CFLAGS} wiringpi_piface_test1.c -o wiringpi_piface_test1 -lwiringPi

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
		echo "\n\e[5mSTD TEST FAILED\e[0m\n"; \
	else \
		echo "\n\e[5mSTD TEST SUCCESS\e[0m\n"; \
	fi

xotest:
	@error_state=false ; \
	for t in $(tests) $(xotests)  ; do \
		echo === XO unit test: $${t} === ; \
		time ./$${t} ; \
		if [ $$? -ne 0 ]; then \
		  error_state=true ; \
		fi ; \
		echo  ; echo  ; \
	done
	if [ "$$error_state" = true ]; then \
		echo "\n\e[5mSTD/XO TEST FAILED\e[0m\n"; \
	else \
		echo "\n\e[5mSTD/XO TEST SUCCESS\e[0m\n"; \
	fi

pifacetest:
	@error_state=false ; \
	for t in $(tests) $(pifacetests)  ; do \
		echo === PiFace unit test: $${t} === ; \
		time ./$${t} ; \
		if [ $$? -ne 0 ]; then \
		  error_state=true ; \
		fi ; \
		echo  ; echo  ; \
	done
	if [ "$$error_state" = true ]; then \
		echo "\n\e[5mPIFACE TEST FAILED\e[0m\n"; \
	else \
		echo "\n\e[5mPIFACE TEST SUCCESS\e[0m\n"; \
	fi

clean: 
	for t in $(tests) $(xotests) $(pifacetests) ; do \
		rm -fv $${t} ; \
	done
