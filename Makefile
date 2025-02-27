all: pam_redirector.so

pam_redirector.so: pam_redirector.c
	gcc -fPIC -Wall -shared -lpam -o pam_unixsock.so pam_unixsock.c

debug: pam_redirector.c
	gcc -fPIC -DDEBUG -Wall -shared -lpam -o pam_unixsock.so pam_runixsock.c

install:
	cp pam_unixsock.so /lib/security/

uninstall:
	rm /lib/security/pam_unixsock.so

clean:
	rm pam_unixsock.so
