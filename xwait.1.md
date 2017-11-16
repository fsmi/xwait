% XWAIT(1)
% Fabian Stumpf <cb@cbcdn.com>; Timo Weingärtner <timo@tiwe.de>
% 2017-11-16

# NAME

xwait - Wait until an X Window is created or mapped.

# SYNOPSIS

**xwait** [**-map**] [**-title** *TITLE*] [**-class** *CLASS*] [*command* [*command arguments*]]

# DESCRIPTION

**xwait** waits for creation of an X window by listening to events on the root
window. It returns after either a CreateNotify event or a MapNotify (by specifying
*--map*) event have been received.
The received events may be filtered further by specifying a window title or class
name via the *--title* and *--class* options.

Optionally, xwait can execute a command after setting up the event filter to
avoid race conditions.

# GENERAL OPTIONS

**-map**
:	Wait for MapNotify events instead of CreateNotify events. The MapNotify event is fired when a window is mapped to the screen to be displayed, while CreateNotify only indicates that a window exists.

**-title** *TITLE*
:	Wait for a window of a specified title instead of reacting to just any window.


**-class** *CLASSNAME*
:	Wait for a window of a specified class name instead of reacting to just any window.

*command* [*command arguments*]
:	Execute a command after starting the event listener. Using this functionality avoids race conditions between starting xwait and an event source.

# EXIT STATUS

Returns 0 on success and other values on error.

# ENVIRONMENT

DISPLAY has to be set, obviously.
