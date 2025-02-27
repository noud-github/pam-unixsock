pam_unixsock
==============

Description
-----------

Want to write Go (or non-C) code for you PAM authentication module? You can't, because you are
trapped inside PAM loaded libraries and your code executes within the space of the process doing the
authentication (i.e. sshd). So if you want modern languages to work you need to get out of the space
as fast as possible, while still being secure. One of those ways if to pass authentication data over
a Unix socket to something else. This project allows for this. If passes the credentials to a Unix
socket, and then you sort it out.

So this code is a pluggable authentication module (PAM) that allows redirection of credentials to a
local Unix socket.

Once "in the unix socket" you need to write an Unix socket server that pulls these out and performs
the validation of said credentials. The protocol is described below and is fairly simplistic.

Protocol
--------

pam_unixsock implements an extremely simple socket protocol whereby pam_unixsock passes a
username, password and a potential second token (2FA) (separated by new line) to the Unix socet
server and then your server simply replies with a 0 or 1:

    [pam_unixsock]   john_smith
    [pam_unixsock]   secret
    ([pam_unixsock]   2fa)
    [your server] 1

Requirements
------------
*   The LibPAM development headers (libpam-dev or libpam0g-dev)
*   A PAM-based system (currently only tested on Linux)

Source Install
--------------

    make clean
    make all

    # make debug will instead build pam_redirector to echo all
    # usernames and passwords to syslog -- useful when
    # troubleshooting, but insecure

    sudo make install

    # which is the same as:
    # cp pam_unixsock.so /lib/security/

Testing
-------

TODO

PAM Configuration
-----------------

Remember, you need to configure PAM to actually use this pam module as well for each service you
want it to authenticate.

Here's an example PAM configuration file to put in /etc/pam.d:

    #%PAM-1.0

    auth optional pam_unix.so
    account optional pam_unix.so

    # @include common-auth
    # @include common-account

    @include common-session

    auth required     pam_unixsock.so /var/run/pam_redirector/socket
    account required  pam_unixsock.so /var/run/pam_redirector/socket

Security Caveats
----------------

1)   Obviously, no authentication occurs between the server and PAM,
     so ensure you restrict access to the socket to root only.
     The example server creates a socket as /tmp/sock, and this
     is VERY INSECURE, for demonstration only.

License
-------

Copyright (c) 2007, 2013 Jamieson Becker

All rights reserved. This package is free software, licensed under the GNU
General Public License (GPL), version 2 or later.

No warranty is provided for this software. Please see the complete terms of
the GNU General Public License for details or contact the author.

Later version Copyright (c) 2025 Miek Gieben
