pam_unixsock
==============
This code is a pluggable authentication module (PAM) that redirects the credentials to a
local Unix socket. The server listening on that socket is then free to do many more complex things,
because it's free from the calling process' address space. In our case we need to do complex things
like doing web requests in Go.

Usage:

    auth       required    pam_unixsock.so [hidden] [no_authtok] [timeout=2] [extra prompt:]

Where the arguments are:

* If `hidden` is set the input to prompt will not be echoed to the screen.
* With `timeout` you can specify how long the module should wait for a response from the server. If
  none is given before the timeout expires this is taken as an *success*. The timeout is in seconds.
* `no_authtok`, do not prompt for the password again.
* If extra prompt text is given, this will be prompted using that text for (see `hidden`) and will
  also be given to the unix socket.

The Unix socket defaults to /var/run/pam_unix.sock The protocol is described below and is fairly simplistic.

Protocol
--------
pam_unixsock implements an extremely simple socket protocol whereby pam_unixsock passes a
username, the PAM service a potential password and second token (2FA, see the extra prompt stuff) (separated by new
line) to the Unix socket and then your server simply replies with a 0 or 1:

    [pam_unixsock]   john_smith\n
    [pam_unixsock]   <pam_service>\n
    [pam_unixsock]   <secret>\n
    [pam_unixsock]   <prompt>\n
    [your server]    1\n

If your server answers within `timeout` (2s by default) with a `1` you are authenticated.

Requirements
------------
* The LibPAM development headers (libpam-dev or libpam0g-dev)
* A PAM-based system (currently only tested on Linux)

Source Install
--------------

    make clean
    make all

    sudo make install

    # which is the same as:
    # cp pam_unixsock.so /lib/security/

Testing
-------

Create a fake pam server called `unixsock`:

~~~
% cat /etc/pam.d/unixsock
#%PAM-1.0
auth       required     pam_unixsock.so
~~~

Open a reader on the socket:

~~~
# nc -lU /var/run/pam_unix.sock
~~~

Use pamtester to authenticate yourself; you can just use a fake password here.

~~~
% sudo pamtester unixsock $USER authenticate
~~~

License
-------
Copyright (c) 2007, 2013 Jamieson Becker

All rights reserved. This package is free software, licensed under the GNU
General Public License (GPL), version 2 or later.

No warranty is provided for this software. Please see the complete terms of
the GNU General Public License for details or contact the author.

Later versions Copyright (c) 2025 Miek Gieben

TODO
-----
* Easer debugging, `debug` option?
* Check if the re-using of the socket works as planned.
