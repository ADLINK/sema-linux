#!/bin/bash

SMBUSDriver=i2c_i801
PCA9535Driver=gpio_pca953x

function testbreak ()
{
  for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo $i > /sys/class/gpio/unexport
  done
	exit 0
}

GPIOPINNoBase=251
GPIOPINNoCount=16
GPIOPINLast=$(( $GPIOPINNoBase + GPIOPINNoCount ))

lsmod | grep $SMBUSDriver > /dev/null
Result=$?

if test $Result -ne 0
  then 
    echo "$SMBUSDriver Driver not yet loaded"
    echo "loading $SMBUSDriver Driver"
    sudo modprobe i2c_i801
    if [ $? ] 
    then
	echo driver loaded successfully
    fi
  fi

lsmod | grep $PCA9535Driver > /dev/null
Result=$?

if test $Result -ne 0
  then 
    echo "$PCA9535Driver Driver not yet loaded"
    exit -1 
  fi

sudo chmod a+rw /sys/class/gpio/export
sudo chmod a+rw /sys/class/gpio/unexport

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo $i > /sys/class/gpio/export
  done

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	sudo chmod a+rw /sys/class/gpio/gpio$i/direction
	sudo chmod a+rw /sys/class/gpio/gpio$i/value
  done

rm /tmp/.TesTrEsuLt 2> /dev/null

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo in > /sys/class/gpio/gpio$i/direction
	cat /sys/class/gpio/gpio$i/direction | grep in > /dev/null
	Result=$?
	if test $Result -ne 1
	then
		echo -e " Changing Pin No $i dir as I/P\t| PASS ">>/tmp/.TesTrEsuLt
	else
		echo -e " Changing Pin No $i dir as I/P\t| FAIL ">>/tmp/.TesTrEsuLt
	fi
  done

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo out > /sys/class/gpio/gpio$i/direction
	cat /sys/class/gpio/gpio$i/direction | grep out > /dev/null
	Result=$?
	if test $Result -ne 1
	then
		echo -e " Changing Pin No $i dir as O/P\t| PASS ">>/tmp/.TesTrEsuLt
	else
		echo -e " Changing Pin No $i dir as O/P\t| FAIL ">>/tmp/.TesTrEsuLt
	fi
  done

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo out > /sys/class/gpio/gpio$i/direction
	echo 1 > /sys/class/gpio/gpio$i/value
	cat /sys/class/gpio/gpio$i/value | grep 1 > /dev/null
	Result=$?
	if test $Result -ne 1
	then
		echo -e " Changing Pin $i value as high\t| PASS ">>/tmp/.TesTrEsuLt
	else
		echo -e "Changing Pin $i value as high\t| FAIL ">>/tmp/.TesTrEsuLt
	fi
  done

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo out > /sys/class/gpio/gpio$i/direction
	echo 0 > /sys/class/gpio/gpio$i/value
	cat /sys/class/gpio/gpio$i/value | grep 0 > /dev/null
	Result=$?
	if test $Result -ne 1
	then
		echo -e "Changing Pin $i value as low\t| PASS ">>/tmp/.TesTrEsuLt
	else
		echo -e "Changing Pin $i value as low\t| FAIL ">> /tmp/.TesTrEsuLt
	fi
  done

trap "testbreak" 2

for (( i=$GPIOPINNoBase; i<$GPIOPINLast; i++ ))
  do
	echo in > /sys/class/gpio/gpio$i/direction
  done

	echo -e "    --------------------------------------------------------------------------------------------------------------------------------------------------------------------" > /tmp/.view
	echo -e "    |\t\t\t\t\t\t\t\t\tPCA 9535 Test Tool\t\t\t\t\t\t\t\t\t       |" >> /tmp/.view
	echo -e "    --------------------------------------------------------------------------------------------------------------------------------------------------------------------" >> /tmp/.view
	echo -e "    |\t\tTest Condition\t\t|Result|\tTest Condition\t\t|Result|\tTest Condition\t\t|Result|\tTest Condition\t\t|Result|" >> /tmp/.view
	echo -e "    --------------------------------------------------------------------------------------------------------------------------------------------------------------------" >> /tmp/.view
	for (( j = 1; j <= $GPIOPINNoCount; j++ ))
	do
		sudo		echo "    |`cat /tmp/.TesTrEsuLt | head -n $j | tail -n 1`|`cat /tmp/.TesTrEsuLt | head -n $(( $j+GPIOPINNoCount )) | tail -n 1`|`cat /tmp/.TesTrEsuLt | head -n $(( $j+2*GPIOPINNoCount )) | tail -n 1`| `cat /tmp/.TesTrEsuLt | head -n $(( $j+3*GPIOPINNoCount )) | tail -n 1`|" >> /tmp/.view
	done
	echo -e "    ---------------------------------------------------------------------------------------------------------------------------------------------------------------------" >> /tmp/.view


while test 1
	do
	
rm /tmp/.TesTrEsuLt 2> /dev/null


	k=0
	for (( j=$GPIOPINNoBase; j<$GPIOPINLast; j++ ))
	do
		for (( i=$(( j + 1 )); i<$(( GPIOPINLast - 1 )); i++ ))
		do
			echo out > /sys/class/gpio/gpio$i/direction
			echo in > /sys/class/gpio/gpio$j/direction

			echo 1 > /sys/class/gpio/gpio$i/value
			cat /sys/class/gpio/gpio$j/value | grep 1 > /dev/null
			c1=$?

			echo 0 > /sys/class/gpio/gpio$i/value
			cat /sys/class/gpio/gpio$j/value | grep 0 > /dev/null
			c2=$?

			echo in > /sys/class/gpio/gpio$i/direction
			echo out > /sys/class/gpio/gpio$j/direction

			echo 1 > /sys/class/gpio/gpio$j/value
			cat /sys/class/gpio/gpio$i/value | grep 1 > /dev/null
			c3=$?

			echo 0 > /sys/class/gpio/gpio$j/value
			cat /sys/class/gpio/gpio$i/value | grep 0 > /dev/null
			c4=$?

			if [ $(( $c1 + $c2 + $c3 + $c4 )) -eq 0 ]
			then
				echo "$i<->$j" >> /tmp/.TesTrEsuLt 
				k=$(( k+1 ))
			fi
		done
	done
	n=$k
	clear
	sudo	cat /tmp/.view
	echo -e	"    |\t       Current Pin status\t|  \tloop\t  |"
	echo "    -------------------------------------------------------"
	for (( i=$GPIOPINNoBase; i < $GPIOPINLast; i++ ))
	do
		echo in > /sys/class/gpio/gpio$i/direction 
		echo -en "    |\t\tPin $i -> `cat /sys/class/gpio/gpio$i/value`\t\t|"
		if [ $k -ne 0 ]
		then
			echo -e "    `cat /tmp/.TesTrEsuLt | head -n $k | tail -n 1`    |"
			k=$(( k-1 ))
		else
			echo "                 |"
		fi
	done
	echo -e "    -------------------------------------------------------"
	echo "press ctrl-c to exit the tests"
done


