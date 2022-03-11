#!/bin/bash

sudo sudo 2> /dev/null

BINDIR=/usr/local/SEMA/bin
BINDIRTEST=/usr/local/SEMA

lsmod | grep i2c_i801 > /dev/null
if [ $?  -eq 1 ]
then
	sudo modprobe i2c_i801
fi

lsmod | grep adl_bmc > /dev/null
if [ $?  -eq 1 ]
then
	echo "BMC (adl-bmc) driver not yet loaded"
	exit -1
fi

if [ ! -e $BINDIRTEST ]
then 
	echo "Application interface not found. install SEMA application"
	exit -1
fi

for line in ls /sys/class/hwmon/hwmon?
do
	ls $line/device/driver 2> /dev/null | grep adl-bmc-fan > /dev/null
	if [ $? -eq 0 ]
	then
		SYSENTRYPATH=$line
	fi
done

sudo chmod a+rw $SYSENTRYPATH/device/pwm?_auto_point?_temp 2> /dev/null
sudo chmod a+rw $SYSENTRYPATH/device/pwm?_* 2> /dev/null

if [ $? -eq 1 ]
then
	echo "FAN control and monitor(adl-bmc-fan) driver not yet loaded"	
	exit -1
fi

for (( m=1; m<5; m++ ))
do
	ls $SYSENTRYPATH/device/pwm${m}_auto_point?_temp >& /dev/null

	if [ $? -eq 0 ]
	then
		case $m in
			1)
				Device="SEMA_EAPI_ID_FAN_CPU"
				;;
			2)
				Device="SEMA_EAPI_ID_FAN_SYSTEM_1"
				;;
			3)
				Device="SEMA_EAPI_ID_FAN_SYSTEM_2"
				;;
			4)
				Device="SEMA_EAPI_ID_FAN_SYSTEM_3"
				;;
		esac

		echo -e "\n$Device"

		for (( l=0; l < 3; l++ ))
		do

			AppFailed=0
			DevEntryFailed=0
			echo $l > $SYSENTRYPATH/device/pwm${m}_auto_channels_temp 2> /dev/null
			if [ `cat $SYSENTRYPATH/device/pwm${m}_auto_channels_temp` -eq $l ]
			then
				AppFailed=1
			fi

			sudo $BINDIR/semaeapi_tool -a  SemaEApiSmartFanSetTempSrc $m $l >& /dev/null
			if [ `sudo $BINDIR/semaeapi_tool -a  SemaEApiSmartFanGetTempSrc $m | cut -d':' -f2 | tr -d ' '` -eq $l ]
			then
				DevEntryFailed=1
			fi

			if [ $AppFailed -eq 1  -o $DevEntryFailed -eq 1 ]
			then
				echo -en "\n Input Temp source "
				case $l in
					0)
						echo "- SEMA_FAN_TEMP_CPU is found"
						TempSource=SEMA_FAN_TEMP_CPU
						;;
					1)
						echo "- SEMA_FAN_SYSTEM is found"
						TempSource=SEMA_FAN_SYSTEM
						;;
					2)
						echo "- SEMA_FAN_SYSTEM_1 is found"
						TempSource=SEMA_FAN_SYSTEM_1
						;;
				esac
			fi

			if [ $AppFailed -eq 0  -a $DevEntryFailed -eq 0 ]
			then
				echo -en "\n Input Temp source "
				case $l in
					0)
						echo "- SEMA_FAN_TEMP_CPU is not found"
						TempSource=SEMA_FAN_TEMP_CPU
						;;
					1)
						echo "- SEMA_FAN_SYSTEM is not found"
						TempSource=SEMA_FAN_SYSTEM
						;;
					2)
						echo "- SEMA_FAN_SYSTEM_1 is not found"
						TempSource=SEMA_FAN_SYSTEM_1
						;;
				esac
				continue
			fi

			echo -en " Changing input resource as $TempSource"

			if [ $AppFailed -eq 0  -a $DevEntryFailed -eq 1 ]
			then
				echo -e "\r Changing input resource as $TempSource - Failed while changing in Application"
			fi

			if [ $AppFailed -eq 1  -a $DevEntryFailed -eq 0 ]
			then
				echo -e "\r Changing input resource as $TempSource - Failed while changing  in SYSFS entry"
			fi

			echo -e "\r Changing input resource as $TempSource - Successfully Changed"

			echo -e "   Starting all basic tests"

			echo -e "     SYSFS -> Application API"

			echo -en "\t1. Possible Temperature values - 0%"

			for (( j=0; j<100; j++ ))
			do
				RESULT=1

				for (( i=1; i<5 ; i++ ))
				do
					echo $j >  $SYSENTRYPATH/device/pwm${m}_auto_point${i}_temp
				done


				TempSetPoint="`sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanGetTempSetpoints $m | cut -d':' -f 2`"

				for i in $TempSetPoint
				do
					if [ $i -ne $j ]
					then 
						RESULT=0
					fi
				done

				if [ $RESULT -ne 1 ]
				then
					echo -e "Failed $i%\r"
				else
					echo -en "\r\t1. Possible Temperature values - $i%"
				fi
			done
			echo -e "\r\t1. Possible Temperature values - Passed"

			echo -ne "\t2. Possible PWM values         - 0%"

			for (( j=0; j<100; j++ ))
			do
				RESULT=1

				for (( i=1; i<5 ; i++ ))
				do
					echo $j >  $SYSENTRYPATH/device/pwm${m}_auto_point${i}_pwm
				done


				PWMSetPoint="`sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanGetPWMSetpoints $m | cut -d':' -f 2`"

				for i in $PWMSetPoint
				do
					if [ $i -ne $j ]
					then 
						RESULT=0
					fi
				done

				if [ $RESULT -ne 1 ]
				then
					echo -e "Failed i%\r"

				else
					echo -ne "\r\t2. Possible PWM values         - $i%"
				fi
			done
			echo -e "\r\t2. Possible PWM values         - Passed"

			echo -e "     Application API -> SYSFS"

			echo -en "\r\t3. Possible Temperature values - 0%"

			for (( j=0; j<100; j++ ))
			do
				RESULT=1
				sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanSetTempSetpoints 1 $j $j $j $j > /dev/null
				TempSetPoint="`sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanGetTempSetpoints 1 | cut -d':' -f 2`"

				k=1;for i in $TempSetPoint
					do
						sudo chmod a+rw $SYSENTRYPATH/device/pwm1_auto_point${k}_temp
						if [ $i -ne `cat $SYSENTRYPATH/device/pwm1_auto_point${k}_temp` ]
						then 
							RESULT=0
						fi
						k=$(( k+1 ))
					done

				if [ $RESULT -ne 1 ]
				then
					echo -e "Failed i%\r"
				else
					echo -en "\r\t3. Possible Temperature values - $i%"
				fi
			done
			echo -e "\r\t3. Possible Temperature values - Passed"

			echo -ne "\t4. Possible PWM values         - 0%"

			for (( j=0; j<=100; j++ ))
			do
				RESULT=1
				sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanSetPWMSetpoints 1 $j $j $j $j > /dev/null
				PWMSetPoint="`sudo $BINDIR/semaeapi_tool -a SemaEApiSmartFanGetPWMSetpoints 1 | cut -d':' -f 2`"

				k=1;for i in $PWMSetPoint
					do
					sudo chmod a+rw $SYSENTRYPATH/device/pwm1_auto_point${k}_pwm
					if [ $i -ne `cat $SYSENTRYPATH/device/pwm1_auto_point${k}_pwm` ]
					then 
						RESULT=0
					fi
					k=$(( k+1 ))
				done

				if [ $RESULT -ne 1 ]
				then
					echo -e "Failed i%\r"
				else
					echo -ne "\r\t4. Possible PWM values         - $i%"
				fi
			done
			echo -e "\r\t4. Possible PWM values         - Passed"
		done
	else
		case $m in
			1)
				echo -e "\nSEMA_EAPI_ID_FAN_CPU not found"
				;;
			2)
				echo -e "\nSEMA_EAPI_ID_FAN_SYSTEM_1 is not found"
				;;
			3)
				echo -e "\nSEMA_EAPI_ID_FAN_SYSTEM_2 is not found"
				;;
			4)
				echo -e "\nSEMA_EAPI_ID_FAN_SYSTEM_3 is not found"
				;;
		esac
	fi
done
exit 0
