//The Header Part
#ifndef FLAG_H_
#define FLAG_H_

#include <stdbool.h>
#include <stdint.h>

bool *flag_bool(const char *name, bool def, const char *desc);
uint64_t *flag_uint64(const char *name, uint64_t def, const char *desc);
const char **flag_str(const char *name, const char *def, const char *desc);
void flag_parse(int argc, char **argv);

#endif // FLAG_H_

/////////////////////////////

//The Implementation Part
#ifndef FLAG_IMPLEMENTATION_
#define FLAG_IMPLEMENTATION_

#endif // FLAG_IMPLEMENTATION_