# cbcleaner
C clipboard(s) cleaning daemon for Ubuntu Linux x64.

This is a C NetBeans project, compiled with GCC 5.4.0 and tested with Ubuntu
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
daemon is active, just execute "ps xj | grep cbcleaner" in a terminal. Logs are
also pushed into /var/log/syslog ("cat /var/log/syslog | grep cbcleaner"). To
stop the daemon, you just have to kill the process with "kill [cbcleaner PID]".

Usage example 1: "./cbcleaner", clipboards will be cleared every 120s with
warning pop-ups activated.

Usage example 2: "./cbcleaner 60 no", clipboards will be cleared every 90s with
warning pop-ups deactivated.

DONATION:
As I share these sources for commercial use too, maybe you could consider
sending me a reward (even a tiny one) to my Ethereum wallet at the address
0x1fEaa1E88203cc13ffE9BAe434385350bBf10868
If so, I would be forever grateful to you and motivated to keep up the good work
for sure :oD Thanks in advance !

FEEDBACK:
You like my work? It helps you? You plan to use/reuse/transform it? You have
suggestions or questions about it? Just want to say "hi"? Let me know your
feedbacks by mail to the address fabvalaaah@laposte.net

DISCLAIMER:
I am not responsible in any way of any consequence of the usage of this piece of
software. You are warned, use it at your own risks.