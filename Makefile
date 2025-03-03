all: pam_unixsock.so

pam_unixsock.so: pam_unixsock.o
	ld -x --shared -o pam_unixsock.so pam_unixsock.o

pam_unixsock.o: pam_unixsock.c
	gcc -fPIC -fno-stack-protector -c pam_unixsock.c -o pam_unixsock.o

clean:
	rm -f pam_unixsock.o pam_unixsock.so
