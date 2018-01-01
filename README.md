# cbcleaner
Clipboards cleaning daemon for Ubuntu Linux x64.

This is a C Netbeans project, compiled with GCC 5.4.0 and tested with Ubuntu
Linux 16.04.3 LTS x64.

Every X seconds, this daemon clears the clipboard, the primary and the secondary
selection atoms of the operating system. The warning pop-ups activation can be
set with the second parameter of the command line (default is "yes"). The amount
of time before advertising default value is 90s but can be customized thanks to
the first parameter of the command line. The amount of time between 2 successive
cleanings will then be of 90s (before advertising) + 30s = 120s by default.

It can be launched through the Startup Applications (use the Dash to find it) of
Ubuntu. I previously added a symbolic link to the binary "cbcleaner" from the
release directory of the NetBeans project into /usr/bin. Only one instance of
the daemon will remain active through several session logins. To check if the
daemon is active, just execute "sudo ps xj | grep cbcleaner" in a terminal. Logs
are also pushed into /var/log/syslog (cat /var/log/syslog | grep cbcleaner). To
stop the daemon, you just have to kill the process with
"sudo kill <cbcleaner pid>"

Usage example 1 : "./cbcleaner", clipboards will be cleared every 120s with
warning pop-ups activated.
Usage example 2 : "./cbcleaner 60 no", clipboards will be cleared every 90s with
warning pop-ups deactivated.