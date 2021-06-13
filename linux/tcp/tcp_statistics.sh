comp()
{
	if [ $2 -gt $(expr $1 + 10) ]
	then
		eval "$3=\"+\""	
	elif [ $2 -gt $(expr $1 - 10) ]
	then
		eval "$3=\" \""
	else
		eval "$3=\"-\""
	fi
}

output()
{
	outnum=`expr $2 - $4`
	recvnum=`expr $1 - $3`
	sum=`expr $outnum + $recvnum`
	if [ $7 -gt 0 ]
	then
		comp $pre_sum $sum "flag"
    fi
	pre_sum=$sum
	eval "$5=$1";eval "$6=$2"
	echo -e "\r" `date "+%Y-%m-%d %H:%M"` "\c"
	printf "%4d %4d %4d %4c\n" $outnum $recvnum $sum $flag
}

index=0
while (true)
	do
	sleep 60
	str=`netstat --statistics | grep "[0-9][0-9]* segments received"`
	tcp_recv=$(expr "$str" : '^ *\([0-9][0-9]*\).*$')
	tcp_out=$(expr "$str" : '^ *\([0-9][0-9]*\).*$')
	output $tcp_recv $tcp_out $recv $out "recv" "out" $index
	index=$(expr $index + 1)
done
