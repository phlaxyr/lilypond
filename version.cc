#include "version.hh"

static char *s = "LilyPond version " VERSIONSTR " compiled on " __DATE__ " at " __DATE__ "\n";

char *
get_version()
{
   return s;
}
