pam_unixsock
==============

This code is a pluggable authentication module (PAM) that redirects the credentials to a
local Unix socket. The server listening on that socket is then free to do many more complex things,
because it's free from the calling process' address space. In our case we need to do complex things
like doing web requests.

usage:

    auth       required    pam_unixsock.so [hidden] [timeout=2] [Extra prompt text:]

Where the arguments are:

* If `hidden` is set the input to prompt will not be echoed to the screen.
* With `timeout` you can specify how long the module should wait for a response from the server. If
  none is given before the timeout expires this is taken as an *success*. The timeout is in seconds.
* If extra prompt text is given, this will be prompted for (see `hidden`) and will also be given
* to the unix socket.

The Uni socket defaults to /var/run/pam_unix.sock


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

    sudo make install

    # which is the same as:
    # cp pam_unixsock.so /lib/security/

Testing
-------

% more /etc/pam.d/unixsock
#%PAM-1.0
auth       required     pam_unixsock.so

# nc -lU /var/run/pam_unix.sock

% sudo pamtester unixsock $USER authenticate

License
-------
Copyright (c) 2007, 2013 Jamieson Becker

All rights reserved. This package is free software, licensed under the GNU
General Public License (GPL), version 2 or later.

No warranty is provided for this software. Please see the complete terms of
the GNU General Public License for details or contact the author.

Later versions Copyright (c) 2025 Miek Gieben
