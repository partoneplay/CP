#########################################################################
# File Name: test.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: 2014年04月12日 星期六 00时52分08秒
#########################################################################
#!/bin/bash

echo "generate test files '10M','100M', and folder 'test'"
if [ ! -d "test" ]; then
	mkdir "test"
fi
cp xcp test/xcp
cd test
dd if=/dev/zero of=10M bs=1K count=10000
dd if=/dev/zero of=100M bs=1K count=10000

echo ""
# test copy files
echo "test copy"

./xcp 10M 10M.copy
cmp 10M 10M.copy
if [ $? = 0 ]; then
	echo "copy right"
else
	echo "copy wrong!"
	exit
fi
./xcp 100M 100M.copy
cmp 100M 100M.copy
if [ $? = 0 ]; then
	echo "copy right"
else
	echo "copy wrong"
	exit
fi

echo ""
echo "test update copy"
./xcp --update 10M 10M.copy
echo "touch 100M"
sleep 1
touch 100M
./xcp --update 100M 100M.copy

echo ""
#test encrypt function
./xcp --encrypt --key=123 10M 10M.cxc
./xcp --encrypt --key=123 100M 100M.cxc

echo ""
# test check function
./xcp --check --key=123 10M.cxc 100M.cxc
echo "use wrong key test"
./xcp --check --key=wrong_key 10M.cxc 100M.cxc
echo "change 10M.cxc to _10M.cxc, 100M.cxc to _100M.cxc"
cp 10M.cxc _10M.cxc
who >> _10M.cxc
cp 100M.cxc _100M.cxc
who >> _100M.cxc
./xcp --check --key=123 _10M.cxc _100M.cxc

echo ""
# test decrypt function
./xcp --decrypt --key=123 10M.cxc _10M
cmp 10M _10M
if [ $? = 0 ]; then
	echo "crypt right"
else
	echo "crypt wrong"
	exit
fi
./xcp --decrypt --key=123 100M.cxc _100M
cmp 100M _100M
if [ $? = 0 ]; then
	echo "crypt right"
else
	echo "crypt wrong"
	exit
fi

echo "complete"
