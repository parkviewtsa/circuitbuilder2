/*
 * CircuitBuilder's universal error code file.
 * Define all possible exit statuses (stati?) here.
 */

#ifndef ERROR_H
#define ERROR_H

#define NOT_FATAL	0
#define FATAL		1

#define CB_SUCCESS		0
#define CB_ERR_SDL_INIT	1	// Something wrong with SDL, specifically init.
#define CB_ERR_QT_INIT	2	// Something wrong with Qt, specifically init.
#define CB_ERR_SDL		3	// Something else wrong with SDL.
#define CB_ERR_QT		4	// Something else wrong with Qt.
#define CR_ERR_NO_FRAME	5	// The renderer failed to render a frame.
#define CR_ERR_NO_BUF	6 // The renderer returned a null buffer.
#define CB_ERR_DNE  7 // Tried to do something to a component that does not exist
#define CB_ERR_MALLOC 8 // Memory allocation failure
#define CB_ERR_RFILE 9 // File read error
#define CB_ERR_WFILE 10 // File write error

#define CB_GROW_ALLOC_FAIL 42 // The renderer tried to allocate memory to
// grow a list and it got a NULL instead. No chance of recovery.

#define CB_ERR_WTF		-1	// For when you just don't know what happened.

#define CB_ERR_WTF		-1	// For when you just don't know what happened.

extern char* get_error_string (int);
