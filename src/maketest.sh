#########################################################################
# File Name: test.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: 2014年04月27日 星期日 20时00分56秒
#########################################################################
#!/bin/bash

if [ ! -f "xcp" ]; then
	echo "Please run 'make' first"
	exit
fi

mkdir test 2>/dev/null
cp xcp test/
cd test

# 准备测试数据
echo "Preparing test data ... "
dd if=/dev/zero of="file 1M" bs=1K count=1000 2>/dev/null
dd if=/dev/zero of="file 10M" bs=1K count=10000 2>/dev/null

mkdir s 2>/dev/null
cp -r /usr/share/s* s
mkdir "s/blank blank" 2>/dev/null
mv "file 1M" "s/blank blank"
mv "file 10M" "s/blank blank"
echo "Prepare data done ."


# 普通拷贝测试
echo "generate md5sum code"
cp -r s s_cp 1>/dev/null
./xcp -m s_cp 1>a
rm -rf s_cp
echo "compare md5sum code"
./xcp s s_cp 1>/dev/null
./xcp -m s_cp 1>b
rm -rf s_cp

sort a b 1>/dev/null
cmp a b 1>/dev/null
if [ $? != 0 ]; then
	echo "Test Copy fail"
	exit
fi
echo "Test Copy OK"
rm a b
 

# 加密解密测试
echo "generate source files md5sum code"
cp -r s s_crypt 1>/dev/null
./xcp -m s_crypt 1>a

echo "encrypt files ..."
./xcp -e -k 123 s_crypt s_crypted 1>/dev/null
rm -rf s_crypt
echo "decrypt files ..."
./xcp -d -k 123 s_crypted s_crypt 1>/dev/null
rm -rf s_crypted

echo "generate decrypted files md5sum code"
./xcp -m s_crypt 1>b
rm -rf s_crypt

echo "compare md5sum code"
sort a b 1>/dev/null
cmp a b 1>/dev/null
if [ $? != 0 ]; then
	echo "Crypt fail"
	exit
fi
echo "Test Crypt OK"
rm a b



#
#
#
#
#

echo "complete"

