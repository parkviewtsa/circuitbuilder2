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
#define CB_ERR_NO_FRAME	5	// The renderer failed to render a frame.
<<<<<<< HEAD
#define CB_ERR_DNE  6 // Tried to do something to a component that does not exist
#define CB_ERR_MALLOC 7 // Memory allocation failure
#define CB_ERR_RFILE 8 // File read error
#define CB_ERR_WFILE 9 // File write error

#define CB_ERR_WTF		-1	// For when you just don't know what happened.

extern char* get_error_string (int);
=======
#define CB_ERR_CR_GROW_ALLOC_FAIL 420 // The renderer tried to allocate memory to grow a list and it got a NULL instead. No chance of recovery.

#define CB_ERR_WTF		-1	// For when you just don't know what happened.

// Turn this off for turn-in.
#define RIDICULOUS_NAMES

char* get_error_string (int code)
{
	switch (code)
	{
		case CB_SUCCESS:
		return "";

		case CB_ERR_SDL_INIT:
		return "The application cannot load because SDL failed to initialize.";
		case CB_ERR_QT_INIT:
		return "The application cannot load because Qt failed to initialize.";
		case CB_ERR_SDL:
		return "SDL encountered a fatal error.";
		case CB_ERR_QT:
		return "Qt encountered a fatal error.";
		case CB_ERR_NO_FRAME:
		return "The renderer failed to render the circuit diagram.";
		case CB_ERR_CR_GROW_ALLOC_FAIL:
		#ifdef RIDICULOUS_NAMES
		return "Holy shit, Batman!";
		#else
		return "Memory allocation failed during a list grow operation.";
		#endif

		case CB_ERR_WTF:
		default:
		return "Unknown error";
	};
};
>>>>>>> df7f25f7033c76dd90d3ff16a181d8e483d2e7d1

#endif
