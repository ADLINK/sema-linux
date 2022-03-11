#$1 control port offset
#$2 data port offset
#$3 offset register
#$4 write value
echo "sudo ./write.sh 0x66 0x62 0x86 0xff"
outb $1 0x81;
echo test
outb $2 $3;
echo test
outb $2 $4;
echo test
