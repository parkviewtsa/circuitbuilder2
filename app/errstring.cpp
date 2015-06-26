#include "../error.h"

char* get_error_string (int code)
{ switch (code) {
	case CB_SUCCESS: return "";
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
  case CB_ERR_DNE:
    return "The requested item does not exist."
  case CB_ERR_MALLOC:
    return "Memory allocation failure"
  case CB_ERR_RFILE:
    return "The file could not be read."
  case CB_ERR_WFILE:
    return "The file could not be written to."

	case CB_ERR_WTF:
		return "Unknown error";
	default:
		return "Unknown error";
}}
