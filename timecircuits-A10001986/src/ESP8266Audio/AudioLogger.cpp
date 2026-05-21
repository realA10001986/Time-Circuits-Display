
#ifdef HAVE_AUDIO_LOGGER

#include "AudioLogger.h"

DevNullOut silencedLogger;
Print* audioLogger = &silencedLogger;

#endif
