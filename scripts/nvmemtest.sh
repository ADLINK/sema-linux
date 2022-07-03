#!/bin/bash

lsmod | grep i2c_i801 > /dev/null

if [ $? -eq  1 ]
then
	echo "Installing i2c_i801 driver"
	sudo modprobe i2c_i801
fi

lsmod | grep adl_bmc > /dev/null

if [ $? -eq  1 ]
then
	echo "ADLINK BMC driver was not found. Please install."
	exit -1
fi


lsmod | grep adl_bmc_nvmem > /dev/null

if [ $? -eq  1 ]
then
	echo "ADLINK NVMEM driver was not found. Please install."
	exit -1
fi


if [ ! -e /usr/local/SEMA ]
then
        echo "SEMA Not installed. For testing SEMA is required."
	exit -1
fi

echo -e "Testing script for ADLINK NV memory driver\n"

filenames=`ls /sys/bus/platform/devices/adl-bmc-nvmem | grep nvmem` 
echo $filenames

count=1
Test=0
sudo /usr/local/SEMA/bin/semaeapi_tool -a SemaEApiStorageCap > /tmp/.test
while read line
do
	if [ `echo $line | cut -d':' -f2 | tr -d ' '` != `sudo cat /sys/bus/platform/devices/adl-bmc-nvmem/capabilities/nvmemcap | head -n $count | tail -n 1 | cut -d' ' -f3` ]
	then
		Test=1
	fi
	count=$(( count + 1 ))
done < /tmp/.test

count=1
while read line
do
	if [ $count -eq 1 ]
	then 
		Size=`echo $line | cut -d':' -f2 | tr -d ' '`
	else
		BlockSize=`echo $line | cut -d':' -f2 | tr -d ' '`
	fi
	count=$(( count + 1 ))
done < /tmp/.test

if [ $Test -eq 1 ]
then
	echo "Capabilities value are differents between application and sysfs entry"
else
	echo "Capabilities value are same between application and sysfs entry"
fi

echo "Memory Size = $Size"
echo "Block Size = $BlockSize"

sudo rm /tmp/testimage 2> /dev/null

echo -e "\nWrite Read Test"

echo -en "Test : Erasing all NV memory index test"
sudo dd if=/dev/zero of=/tmp/testimage bs=$Size count=1 2> /dev/null
sudo dd if=/tmp/testimage of=`ls /sys/bus/nvmem/devices/$filenames/nvmem` bs=$Size count=1 2> /dev/null
sudo dd if=`ls /sys/bus/nvmem/devices/$filenames/nvmem` of=/tmp/testimageop bs=$Size count=1 2> /dev/null
diff /tmp/testimage /tmp/testimageop  2> /dev/null

if [ $? -ne 0 ]
then
	echo " is Failed"
else
	echo " is Passed"
fi

sudo rm /tmp/testimage 2> /dev/null

echo -en "Test : Writing all NV memory index test"
sudo dd if=/dev/urandom of=/tmp/testimage bs=$Size count=1 2> /dev/null
sudo dd if=/tmp/testimage of=`ls /sys/bus/nvmem/devices/$filenames/nvmem` bs=$Size count=1 2> /dev/null
sudo dd if=`ls /sys/bus/nvmem/devices/$filenames/nvmem` of=/tmp/testimageop bs=$Size count=1 2> /dev/null
diff /tmp/testimage /tmp/testimageop > /dev/null

if [ $? -ne 0 ]
then
	echo " is Failed"
else
	echo " is Passed"
fi

echo -e "\nPattern Test Started"

for j in "a" "1" "g"
do
	echo -en "$j => "
	sudo rm /tmp/testimage 2> /dev/null
	for (( i=0; i<$Size; i++ ))
	do
		echo -en $j >> /tmp/testimage
	done

	sudo dd if=/tmp/testimage of=`ls /sys/bus/nvmem/devices/$filenames/nvmem` bs=$Size count=1 2> /dev/null
	sudo dd if=`ls /sys/bus/nvmem/devices/$filenames/nvmem` of=/tmp/testimageop bs=$Size count=1 2> /dev/null
	diff /tmp/testimage /tmp/testimageop > /dev/null

	if [ $? -ne 0 ]
	then
		echo "Test is failed"
	else
		echo "Test is Passed"
	fi
done

echo -en "\nWriting memry index via application interface and reading via sysfs entry "

sudo rm /tmp/testimage 2> /dev/null
for (( i=0; i<$Size; i++ ))
do
	sudo /usr/local/SEMA/bin/semaeapi_tool -a SemaEApiStorageAreaWrite $i  "tttt" > /dev/null
	echo -en "$j$j$j$j" >> /tmp/testimage
	i=$(( i + 3 ))
done

sudo dd if=/tmp/testimage of=`ls /sys/bus/nvmem/devices/$filenames/nvmem` bs=$Size count=1 2> /dev/null
sudo dd if=`ls /sys/bus/nvmem/devices/$filenames/nvmem` of=/tmp/testimageop bs=$Size count=1 2> /dev/null
diff /tmp/testimage /tmp/testimageop > /dev/null

if [ $? -ne 0 ]
then
	echo "Test is failed"
else
	echo "Test is Passed"
fi

