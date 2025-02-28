pam_unixsock
==============

Usage:

    auth       required    pam_unixsock.so [prompt="..." [hidden]] [timeout=2s /var/run/unix.sock

Where the arguments are:

* The Unix socket to write to, something should be listening, otherwise it will grant
    access after a 2s timeout
* The `prompt` argument is optional, if set unixsock will print "prompt" and also write the input to the Unix
    socket. If `hidden` is set the input to prompt will not be echoed to the screen.
* With `timeout` you can specify how long the module should wait for a response from the server. If
  none is given before the timeout expires this is taken as an *success*.

This code is a pluggable authentication module (PAM) that redirects the credentials to a
local Unix socket. The server listening on that socket is then free to do many more complex things,
because it's free from the calling process' address space. In our case we need to do complex things
like doing web requests.

The protocol is described below and is fairly simplistic.

Protocol
--------
pam_unixsock implements an extremely simple socket protocol whereby pam_unixsock passes a
username, password and a potential second token (2FA, see `prompt`) (separated by new line) to the
Unix socket and then your server simply replies with a 0 or 1:

    [pam_unixsock]   john_smith\n
    [pam_unixsock]   secret\n
    [pam_unixsock]   prompt\n
    [your server]    1\n

If your server doesn't answer within `timeout` (2s by default), a `1` is assumed. If there was no
prompt the third line written will be empty.

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

Later versions Copyright (c) 2025 Miek Gieben
