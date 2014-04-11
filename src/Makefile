all:
	gcc main.c xcp.c func.c aes.c md5.c winopt.c -o xcp

test:
	gcc main.c xcp.c func.c aes.c md5.c winopt.c -o xcp
	sh maketest.sh

clean:
	rm xcp
	rm -rf test
