
KCFLAGS := -Wno-error
export KCFLAGS

SEMA_OBJS = $(patsubst %.c,%.o,$(wildcard lib/*.c))
WDOG_OBJS = $(patsubst %.c,%.o,$(wildcard watchdogtest/*.c))
APP_OBJS = $(patsubst %.c,%.o,$(wildcard app/*.c))
obj-m := driver/adl-bmc.o \
         driver/adl-bmc-bklight.o \
         driver/adl-bmc-wdt.o \
         driver/adl-bmc-boardinfo.o \
         adl-bmc-nvmem-sec.o \
         adl-bmc-nvmem.o \
         driver/adl-bmc-vm.o \
         driver/adl-bmc-hwmon.o \
         driver/adl-bmc-i2c.o \
         driver/adl-bmc-gpio.o

adl-bmc-nvmem-sec-m := driver/adl-bmc-nvmem-sec.o driver/nvmem-common.o	 
adl-bmc-nvmem-m := driver/adl-bmc-nvmem.o driver/nvmem-common.o	 
all: libsema.so semautil wdogtest modules

driver: modules

libsema.so: $(SEMA_OBJS)
	@gcc -shared -fPIC -g -o lib/$@ $^

modules:
	@make -C /lib/modules/`uname -r`/build M=`pwd` $(KCFLAGS) $@

clean: driver_clean app_clean

install: all driver_install app_install

driver_install:
	@FILE=/lib/modules/`uname -r`/build/certs; if [ ! -f $FILE ]; then mkdir -p /lib/modules/`uname -r`/build/certs; fi 
	@openssl req -new -nodes -utf8 -sha512 -days 36500 -batch -x509 -config x509.genkey -outform PEM -out signing_key.x509 -keyout signing_key.pem > /dev/null
	@cp signing_key.pem /lib/modules/`uname -r`/build/certs/
	@cp signing_key.x509 /lib/modules/`uname -r`/build/certs/
	@make -C /lib/modules/`uname -r`/build M=`pwd` $(KCFLAGS) modules_install
	@depmod -a

app_install:
	@cp lib/libsema.so /usr/lib
	@cp wdogtest semautil /usr/bin

driver_clean:
	@make -C /lib/modules/`uname -r`/build M=`pwd` clean
	@if [ -d "/lib/modules/`uname -r`/extra" ]; then rm -rf /lib/modules/`uname -r`/extra/adl*; fi
	@if [ -d "/lib/modules/`uname -r`/extra/driver" ]; then rm -rf /lib/modules/`uname -r`/extra/driver/adl*; fi
	@if [ -d "/lib/modules/`uname -r`/updates" ]; then rm -rf /lib/modules/`uname -r`/updates/adl*; fi
	@if [ -d "/lib/modules/`uname -r`/updates/driver" ]; then rm -rf /lib/modules/`uname -r`/updates/driver/adl*; fi

app_clean:
	@rm -f semautil wdogtest app/*.o lib/*.o lib/*.so

semautil: $(APP_OBJS)
	@gcc -g -o $@ $^ -Llib -lsema -luuid

wdogtest: $(WDOG_OBJS)
	@gcc $^ -g -o $@

lib/%.o: lib/%.c
	@gcc -Wall -I lib -g -fPIC -c $< -o $@

app/%.o: app/%.c
	@gcc -Wall -I lib -g -fPIC -c $< -o $@

watchdogtest/%.o: watchdogtest/%.c
	@gcc -Wall -I lib -g -fPIC -c $< -o $@
