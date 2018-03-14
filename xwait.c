/*
 * Copyright (c) 2014, Fabian Stumpf <cb@cbcdn.com>
 * Copyright (c) 2014, Timo Weingärtner <timo@tiwe.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <stdbool.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

bool matchExtended(Display* display, Window w, char* class, char* title) {
	char* result;
	XClassHint class_hints;
	bool rv = true;

	//Sanity check
	if (!title && !class) {
		return false;
	}

	if(title){
		if (!XFetchName(display, w, &result) || !result) {
			return false;
		}

		if (strcmp(title, result)) {
			XFree(result);
			return false;
		}
		XFree(result);
	}

	if(class){
		if (!XGetClassHint(display, w, &class_hints) || !class_hints.res_class){
			return false;
		}

		if (strcmp(class, class_hints.res_class)){
			rv = false;
		}

		XFree(class_hints.res_name);
		XFree(class_hints.res_class);
	}

	return rv;
}


// chars should also be const, but see execvp(3p)
int main(int argc, char **argv) {
	Display* dpy;
	Window root, w = 0;
	char* title = NULL, *class = NULL;
	int eventType = CreateNotify, format = 0;
	unsigned long items, bytes;
	Atom pid_type, net_wm_pid;
	bool print = false;
	pid_t pid = 0, window_pid = 0;

	//Open X11 connection
	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Failed to open display, is DISPLAY set?\n");
		exit(EX_UNAVAILABLE);
	}

	//Get the Root window to intercept global events
	if (!(root = RootWindow(dpy, DefaultScreen(dpy)))) {
		fprintf(stderr, "Root window could not be opened\n");
		exit(EX_UNAVAILABLE);
	}

	net_wm_pid = XInternAtom(dpy, "_NET_WM_PID", True);

	//Select which events we want to handle
   	XSelectInput(dpy, root, SubstructureNotifyMask);

	//Skip argument 0
	argv++;
	argc--;

	//Parse internal arguments
	for (; argc > 0 && argv != NULL && **argv == '-'; argv++, argc--) {
		if (!strcmp(*argv, "-title")) {
			argv++;
			argc--;
			title = *argv;
			continue;
		}

		if (!strcmp(*argv, "-class")) {
			argv++;
			argc--;
			class = *argv;
			continue;
		}

		if (!strcmp(*argv, "-map")) {
			eventType = MapNotify;
			continue;
		}

		if (!strcmp(*argv, "-print")) {
			print = true;
			continue;
		}
		break;
	}

	//Execute additional program if requested
	if (argc > 0) {
		pid = fork();
		switch (pid) {
			case -1:
				perror("fork");
				exit(EX_UNAVAILABLE);
			case 0:
				execvp(argv[0], argv);
				perror("execvp");
				exit(EX_UNAVAILABLE);
		}
	}

	//Wait for the requested event on the Root window
	XEvent event;
	for (;;) {
		XNextEvent(dpy, &event);
		if (event.type == eventType
				//Ignore modal windows
				&& ((eventType == CreateNotify && !event.xcreatewindow.override_redirect)
					|| (eventType == MapNotify && !event.xmap.override_redirect))) {
			w = (eventType == CreateNotify) ? event.xcreatewindow.window : event.xmap.window;
			//If _NET_WM_PID supported and child pid does not match, continue
			if (pid && net_wm_pid != None &&
					XGetWindowProperty(dpy, w, net_wm_pid,
					0, sizeof(window_pid) / 4, False, XA_CARDINAL,
					&pid_type, &format, &items, &bytes,
					(unsigned char**) &window_pid) == Success && pid_type == XA_CARDINAL){
				if(window_pid != pid){
					continue;
				}
			}

			//Optionally, match window title and class
			if (title || class) {
				if (matchExtended(dpy, w, class, title)) {
					break;
				}
				continue;
			}
			break;
		}
	}

	if(print){
		printf("%zu", w);
	}
	XCloseDisplay(dpy);
	return 0;
}
