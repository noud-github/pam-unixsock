%%%
title="pam_unixsock 8"
area="Linux-PAM Manual"
date=2025-03-02
[[author]]
fullname="Miek Gieben"
%%%

# NAME

pam_unixsock - PAM module to send credentials to a unix socket

# Synopsis

**pam_unixsock.so** [**hidden**] [**timeout**] [**debug**]

# Description

This code is a pluggable authentication module (PAM) that redirects the credentials to a local Unix
socket. The server listening on that socket is then free to do many more complex things, because
it's free from the calling process' address space. The Unix socket defaults to
`/var/run/pam_unix.sock`. The protocol is described below and is fairly simplistic. If *PROMPT* is
given, the text is used to prompt the user for another authentication token.

# Options

**debug**
:  print debug information

**timeout**
:  set the timeout in seconds for how long to wait for a response from the server, the default is
   `timeout=2`

**hidden**
:  when prompting for another authentication token, hide the input


# Protocol

**pam_unixsock** implements an extremely simple socket protocol whereby it passes an username, the
PAM service, a potential password or second token (i.e. **PROMPT**) (separated by new lines) to the
Unix socket and then your server simply replies with a 0 or 1:

    [pam_unixsock]   john_smith\n
    [pam_unixsock]   <pam_service>\n
    [pam_unixsock]   <prompt>\n
    [your server]    1\n

If your server answers within `timeout` (2 by default) with a `1` you are authenticated.

# Configuration

With Ubuntu (24.04), in `/etc/pam.d/sshd`:

    # Standard Un*x authentication.
    @include common-auth

    # add this line
    auth required pam_unixsock.so debug Enter 2FA token

## SSH

In the `sshd` configuration be sure to add:

~~~
KbdInteractiveAuthentication yes
UsePAM yes
~~~

Note that with public key authentication this is bypassed, and you log in without being asked for a
second token.

# Notes

In Fedora the socket can't be written to by sshd because selinux does not allow it.

# Author
