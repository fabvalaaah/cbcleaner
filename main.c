/*
 * MIT License
 * 
 * Copyright (c) 2017 Fabvalaaah - fabvalaaah@laposte.net
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * DONATION:
 * As I share these sources for commercial use too, maybe you could consider
 * sending me a reward (even a tiny one) to my Ethereum wallet at the address
 * 0x1fEaa1E88203cc13ffE9BAe434385350bBf10868
 * If so, I would be forever grateful to you and motivated to keep up the good
 * work for sure :oD Thanks in advance !
 * 
 * FEEDBACK:
 * You like my work? It helps you? You plan to use/reuse/transform it? You have
 * suggestions or questions about it? Just want to say "hi"? Let me know your
 * feedbacks by mail to the address fabvalaaah@laposte.net
 * 
 * DISCLAIMER:
 * I am not responsible in any way of any consequence of the usage
 * of this piece of software. You are warned, use it at your own risks.
 */

/* 
 * File:   main.c
 * Author: Fabvalaaah
 *
 * 12/16/2017
 */

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 10
#define CURRENT_DIR "/"
#define PID_FILE "/tmp/cbcleaner.pid"
#define DELTA 90
#define ADVERT_DELTA 30
#define ADVERT_CMD "notify-send -i dialog-warning \"Clipboards will be cleared in 30s\" \"cbcleaner\""
#define SUCCESS_CMD "notify-send -i dialog-information \"Clipboards are now cleared\" \"cbcleaner\""

static int fileHandle = -1;

static void endDaemon() {
    if (-1 != fileHandle) {
        close(fileHandle);
        fileHandle = -1;
        remove(PID_FILE);
    }
    syslog(LOG_INFO, "Daemon terminated\n");
    closelog();
}

static void handleSignal(int sig) {
    switch (sig) {
        case SIGTSTP:
        case SIGKILL:
        case SIGINT:
        case SIGTERM:
        {
            endDaemon();
            exit(EXIT_SUCCESS);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void daemonize() {
    int pid;
    struct sigaction newSigAction;
    sigset_t newSigSet;
    char str[BUFFER_SIZE];

    /* Fork from the parent process (child creation) */
    pid = fork();
    if (0 > pid) {
        syslog(LOG_ERR, "Could not fork\n");
        endDaemon();
        exit(EXIT_FAILURE);
    }
    if (0 < pid) {
        syslog(LOG_NOTICE, "Child process created: %d\n", pid);
        exit(EXIT_SUCCESS);
    }
    /* ------- */

    /* Create a new session (detach process from CTTY) */
    if (0 > setsid()) {
        syslog(LOG_ERR, "Could not get a new process group\n");
        endDaemon();
        exit(EXIT_FAILURE);
    }
    /* ------- */

    /* Blocked signals definition */
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGTTOU);
    sigaddset(&newSigSet, SIGTTIN);
    sigaddset(&newSigSet, SIGCHLD);
    sigaddset(&newSigSet, SIGHUP);
    sigprocmask(SIG_BLOCK, &newSigSet, NULL);
    /* ------- */

    /* Handled signals definition */
    newSigAction.sa_handler = handleSignal;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;
    sigaction(SIGTSTP, &newSigAction, NULL);
    sigaction(SIGKILL, &newSigAction, NULL);
    sigaction(SIGINT, &newSigAction, NULL);
    sigaction(SIGTERM, &newSigAction, NULL);
    /* ------- */

    /* Fork from child process (grandchild creation) */
    pid = fork();
    if (0 > pid) {
        syslog(LOG_ERR, "Could not fork\n");
        endDaemon();
        exit(EXIT_FAILURE);
    }
    if (0 < pid) {
        syslog(LOG_NOTICE, "Grandchild process created: %d\n", pid);
        exit(EXIT_SUCCESS);
    }
    /* ------- */

    /* Change the file mode mask */
    umask(0027);
    /* ------- */

    /* Change the current working directory */
    if (-1 == chdir(CURRENT_DIR)) {
        syslog(LOG_WARNING, "Current directory definition failed\n");
    }
    /* ------- */

    /* Close all opened file descriptors */
    for (fileHandle = sysconf(_SC_OPEN_MAX); fileHandle >= 0; fileHandle--) {
        close(fileHandle);
    }
    /* ------- */

    /* Close the default I/O streams */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    /* ------- */

    /* Open/create and write PID into a unique PID lock file */
    fileHandle = open(PID_FILE, O_RDWR | O_CREAT, 00600);
    if (-1 == fileHandle) {
        syslog(LOG_ERR, "Could not open PID lock file %s, exiting\n", PID_FILE);
        exit(EXIT_FAILURE);
    }
    if (-1 == lockf(fileHandle, F_TEST, 0)) {
        syslog(LOG_ERR, "PID lock file %s is already locked, exiting\n",
                PID_FILE);
        exit(EXIT_FAILURE);
    }
    memset(str, '\0', BUFFER_SIZE);
    snprintf(str, BUFFER_SIZE, "%d\n", getpid());
    if (strlen(str) != write(fileHandle, str, strlen(str))) {
        syslog(LOG_ERR, "Could not write into PID lock file %s, exiting\n",
                PID_FILE);
        exit(EXIT_FAILURE);
    }
    if (-1 == lockf(fileHandle, F_TLOCK, 0)) {
        syslog(LOG_ERR, "Could not lock PID lock file %s, exiting\n", PID_FILE);
        exit(EXIT_FAILURE);
    }
    /* ------- */
}

static int clearClipboards() {
    Display* display = NULL;
    Atom selection;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        syslog(LOG_WARNING, "Cannot open default display\n");
        return EXIT_FAILURE;
    }

    selection = XInternAtom(display, "CLIPBOARD", False);
    XSetSelectionOwner(display, selection, None, CurrentTime);
    selection = XInternAtom(display, "PRIMARY", False);
    XSetSelectionOwner(display, selection, None, CurrentTime);
    selection = XInternAtom(display, "SECONDARY", False);
    XSetSelectionOwner(display, selection, None, CurrentTime);
    XSync(display, False);
    syslog(LOG_INFO, "Clipboards cleared\n");

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    unsigned long delta;
    int retVal = EXIT_SUCCESS;
    bool advert = true;

    /* The management of the command line parameters is minimalistic */
    if (3 == argc && argv[1] && argv[2]) {
        delta = strtoul(argv[1], NULL, 10);
        if (!strncmp(argv[2], "no", 2)) {
            advert = false;
        }
    }
    if (!delta) {
        delta = DELTA;
    }
    /* ------- */

    openlog("cbcleaner", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon is starting (interval set to %lu seconds)\n",
            delta);

    daemonize();

    syslog(LOG_INFO, "Daemon started\n");
    while (1) {
        retVal = clearClipboards();
        if (advert && system(SUCCESS_CMD)) {
            syslog(LOG_ERR, "Failed to notify about clipboards clearance\n");
            endDaemon();
        }
        sleep(delta);
        if (advert && system(ADVERT_CMD)) {
            syslog(LOG_ERR, "Failed to warn about clipboards clearance\n");
            endDaemon();
        }
        sleep(ADVERT_DELTA);
    }

    return retVal; /* To satisfy the "pedantic" option */
}