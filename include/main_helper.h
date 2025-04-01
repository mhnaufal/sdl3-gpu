// NOT A PROJECT TO BE TAKEN SERIOUSLY

#pragma once

#include "FMOD/fmod_errors.h"
#include "SDL3/include/SDL3/SDL_error.h"

/****************************************/
// Helper Macros & Functions
/****************************************/

#define SDL_CHECK_ERROR(CTX, ERROR_MSG, RETURN)                                                    \
    CTX.error = SDL_GetError();                                                                    \
    if (*CTX.error != NULL) {                                                                      \
        fprintf(stderr, ERROR_MSG);                                                                \
        fprintf(stderr, CTX.error);                                                                \
        return RETURN;                                                                             \
    }

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

#define FMOD_CHECK_ERROR(x)                                                                        \
    if (x != FMOD_OK) {                                                                            \
        fprintf(stderr, FMOD_ErrorString(x));                                                      \
    }
