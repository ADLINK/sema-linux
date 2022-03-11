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
	echo "ADLINK BMC driver was not found. Please load."
	exit -1
fi


lsmod | grep adl_bmc_vm > /dev/null

if [ $? -eq  1 ]
then
	echo "ADLINK volatage monotor driver was not found. Please load."
	exit -1
fi

if [ ! -e /usr/local/SEMA ]
then
        echo "SEMA Not installed. For testing SEMA is required."
	exit -1
fi



Count=`ls /sys/class/regulator/regulator.*/device/driver/adl-bmc-vm | tr -cd ':' | wc -c`


echo -e "Testing script for ADLINK volatage monitor driver\n" > /tmp/.view1

for (( i=1; i<=$Count; i++ ))
do
	IsTestFailed=0
	Value=`ls /sys/class/regulator/regulator.*/device/driver/adl-bmc-vm | grep regulator. | head -n $i | tail -n 1 | tr -d ':' | cut -d'.' -f2 | cut -d'/' -f1`

	cat /sys/class/regulator/regulator.$Value/name > /dev/null
	if [ !  $? -eq 0 ]
	then	
		IsTestFailed=1
	fi
	cat /sys/class/regulator/regulator.$Value/microvolts > /dev/null
	if [ ! $? -eq 0 ]
	then	
		IsTestFailed=1
	fi
	if [ $IsTestFailed -eq 1 ]
	then
		echo "Test Failed at Regulator.$Value" >> /tmp/.view1
	else
		echo "Reading Description and value test Passed for Regulator.$Value" >> /tmp/.view1
	fi
done
echo >> /tmp/.view1

while test 1
do
	echo "-----------------------------------------------------------------------------------------" > /tmp/.view
	echo -e "| Regulator No	| Descriptio : value 	| Description : value	|\tComparision\t|" >> /tmp/.view
	echo "-----------------------------------------------------------------------------------------" >> /tmp/.view

	for (( i=1; i<=$Count; i++ ))
	do
		echo -en "| Regulator $i \t| " >> /tmp/.view
		Value=`ls /sys/class/regulator/regulator.*/device/driver/adl-bmc-vm | grep regulator. | head -n $i | tail -n 1 | tr -d ':' | cut -d'.' -f2 | cut -d'/' -f1`
		echo -en "`cat /sys/class/regulator/regulator.$Value/name`:\t" 2> /dev/null >> /tmp/.view

		Value1=`cat /sys/class/regulator/regulator.$Value/microvolts`

		echo -en "$Value1 \t|" 2> /dev/null >> /tmp/.view
		Output=`sudo /usr/local/SEMA/bin/semaeapi_tool -a SemaEApiBoardGetVoltageMonitor $i`

		test=0
		echo $Output | grep "information failed" > /dev/null
		if [ $? -eq 0 ]
		then
			echo -en " Failed:\tFailed\t|" >> /tmp/.view
			test=1
		else
			echo -en " `echo $Output | cut -d':' -f 3 | tr -d ' '`" >> /tmp/.view
			Need=`echo $Output | cut -d':' -f 3 | tr -d ' ' | wc -c`
			if [ $Need -eq 7 ]
			then
				echo -ne ":\t" >> /tmp/.view
			else
				echo -en "\t:\t" >> /tmp/.view
			fi
			Value2=`echo $Output | cut -d':' -f 2 | cut -d' ' -f 2 | tr -d ' '`
			echo -ne "$Value2\t|" >> /tmp/.view
		fi

		if [ $test -eq 1 ]
		then
			echo -e "\t Invalid\t|" >> /tmp/.view
		else
			if [ $Value1 -eq $Value2 ]
			then
				echo -e "\t Same   \t|" >> /tmp/.view
			else
				echo -e "\t differs\t|" >> /tmp/.view
			fi
		fi
	done
	echo -e "-----------------------------------------------------------------------------------------" >> /tmp/.view
	clear
	cat /tmp/.view1
	echo -e "Status\n"
	cat /tmp/.view
done
