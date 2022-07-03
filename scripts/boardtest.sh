#!/bin/bash

sudo sudo 2> /dev/null

BINDIR=/usr/local/SEMA/bin

lsmod | grep i2c_i801 > /dev/null
if [ $?  -eq 1 ]
then
	sudo modprobe i2c_i801
fi

lsmod | grep adl_bmc > /dev/null
if [ $?  -eq 1 ]
then
	echo "BMC driver Not loaded."
	exit
fi


lsmod | grep adl_bmc_boardinfo > /dev/null
if [ $?  -eq 1 ]
then
	echo "Board Information driver Not loaded."
	exit	
fi

if [ ! -e /usr/local/SEMA ]
then
	echo "SEMA not found. Install SEMA utilities."
	exit
else
	SEMAPATH=/usr/local/SEMA/bin
fi

echo Tests:

echo -n Test : SemaEApiBoardGetStringA reading in SYS entry and application inteface - 

if [ `cat /sys/bus/platform/devices/adl-bmc-boardinfo/information/board_name  | cut -d' ' -f2 | tr -d ' ' ` = `sudo $SEMAPATH/semaeapi_tool -a SemaEApiBoardGetStringA 2` ]
then
	echo " Passed"
else
	echo " Failed"
	exit 
fi

sudo chmod a+rw /sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des

for i in {00..21}
do
	echo -en "Test : Exception code $i Read & Write in Sys fs Entry and Read using Application Interface - "
	echo $i > /sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des 2> /dev/null
	if [ $? -eq 0 ]
	then
		if [ `cat /sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des` = `sudo $SEMAPATH/semaeapi_tool -a SemaEApiBoardGetExceptionDescription $i` ]
		then
			echo "Passed"
		else
			echo Failed
		fi
	else
		echo "Failed"
	fi
done

echo -e "\nException List\nNo ->   value\n--------------------"
for i in {00..21}
do
	echo $i > /sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des 2> /dev/null
	echo -en "$i -> "
	cat /sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des
done
