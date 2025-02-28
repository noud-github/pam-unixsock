all: pam_unixsock.so

pam_unixsock.so: main.go
	go build -buildmode=c-shared -o pam_unixsock.so
