#!/bin/bash

stress() {
	./webserv _bsugerTests/configs/default.conf & SERVER_PID=$!
	sleep 1
	echo -n "Start stress test on $1"
	siege -c50 -b -t10S $1 > siege_output.txt 2>&1
	kill -2 $SERVER_PID 
	wait $SERVER_PID 2>/dev/null
	AVAILABILITY=$(grep -i "availability" siege_output.txt | awk '{print $2}' | tr -d ',')
	echo "Availability : $AVAILABILITY"
	rm -rf siege_output.txt
}

echo "Launch a serie of tests with siege"
echo "Siege is launched with the following parameters; -c50 -b -t10S"
echo "-c50 => 50 concurrent number of users"
echo "-b => no delay between iterations"
echo "-t10S => 10 seconds tests"
echo ""

stress "http://127.0.0.1:8002/"
sleep 1
stress "http://127.0.0.1:8002/cgi-bin/"
sleep 1
stress "http://127.0.0.1:8002/upload"
