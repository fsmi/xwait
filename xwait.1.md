% XWAIT(1)
% Fabian Stumpf <cb@cbcdn.com>; Timo Weingärtner <timo@tiwe.de>
% 2014-01-23

# NAME

xwait - wait for creation of X window

# SYNOPSIS

*xwait* [*command* [*command arguments*]]

# DESCRIPTION

xwait waits for creation of any X window and optionally starts a command after setting up the event filter.

# EXIT STATUS

Returns 0 on success and other values on error.

# ENVIRONMENT

DISPLAY has to be set obviously.
