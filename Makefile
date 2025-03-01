all: pam_unixsock.so

pam_unixsock.o: pam_unixsock.c
	gcc -fPIC -fno-stack-protector -c pam_unixsock.c -o pam_unixsock.o
	ld -x --shared -o pam_unixsock.so pam_unixsock.o

install: pam_unixsock.so
	cp pam_unixsock.so /lib64/security

unstall: pam_unixsock.so
	rm /lib64/security/pam_unixsock.so

clean:
	rm pam_unixsock.o pam_unixsock.so
