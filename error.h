/*
 * CircuitBuilder's universal error code file.
 * Define all possible exit statuses (stati?) here.
 */
 
#ifndef ERROR_H
#define ERROR_H

#define CB_SUCCESS		0
#define CB_ERR_SDL		1	// Something wrong with SDL, specifically init.
#define CB_ERR_QT		2	// Something wrong with Qt, specifically init.

#define CB_ERR_WTF		-1	// For when you just don't know what happened.	

#endif