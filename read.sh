test=0;
while test ! $test -eq 256 ;
do outb 0x66 0x80;
outb 0x62 $test;
value=`inb 0x62`;
printf "%02x " $value; 
test=`echo "$test + 1" | bc`;
newline=`echo "$test % 16" | bc`
if test $newline -eq 0; then
        echo 
fi
done

echo
echo

test=0;
while test ! $test -eq 256 ;
do outb 0x6C 0x80; 
outb 0x68 $test; 
value=`inb 0x68`; 
printf "%02x " $value; 
test=`echo "$test + 1" | bc`;
newline=`echo "$test % 16" | bc`
if test $newline -eq 0; then
        echo
fi
done
     
