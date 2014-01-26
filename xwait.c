/*
 * Copyright (c) 2014, Fabian Stumpf <fb@fbcdn.com>
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
#include <X11/Xlib.h>

// chars should also be const, but see execvp(3p)
int main(int const argc, char * const * const argv) {
	Display * dpy;
	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Failed to open display, is DISPLAY set?\n");
		exit(EX_UNAVAILABLE);
	}
	
	Window w;
	if (!(w = RootWindow(dpy, DefaultScreen(dpy)))) {
		fprintf(stderr, "Root window could not be opened\n");
		exit(EX_UNAVAILABLE);
	}

   	XSelectInput(dpy, w, SubstructureNotifyMask);

	if (argc > 1)
		switch (fork()) {
			case -1:
				perror("fork");
				exit(EX_UNAVAILABLE);
			case 0:
				execvp(argv[1], argv + 1);
				perror("execvp");
				exit(EX_UNAVAILABLE);
		}

	XEvent event;
	for (;;) {
		XNextEvent(dpy, &event);
		if (event.type == CreateNotify)
			break;
	}
	
	XCloseDisplay(dpy);
	return 0;
}
