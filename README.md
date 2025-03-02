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
