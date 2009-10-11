#ifdef __APPLE__
 #include "config_mac.h"
#elif __GNUC__
 #include "config_linux.h"
#elif _MSC_VER
 #include "windows/config.h"
#endif
