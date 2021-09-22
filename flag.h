//The Header Part
#ifndef FLAG_H_
#define FLAG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef enum
{
    FLAG_BOOL,
    FLAG_UINT64,
    FLAG_STR,
} Flag_Type;

void *flag_new(Flag_Type type, const char *name, const char *desc);

#define flag_bool(name, desc) ((bool *)(flag_new(FLAG_BOOL, name, desc)))
#define flag_uint64(name, desc) ((uint64_t *)(flag_new(FLAG_UINT64, name, desc)))
#define flag_str(name, desc) ((const char **)(flag_new(FLAG_STR, name, desc)))

void flag_parse(int argc, char **argv);
void flag_print_help(FILE *stream);

#endif // FLAG_H_

/////////////////////////////////////////////////////////////////////////////////////

//The Implementation Part
#ifndef FLAG_IMPLEMENTATION_
#define FLAG_IMPLEMENTATION_

typedef struct
{
    Flag_Type type;
    const char *name;
    const char *desc;
    uintptr_t data;
} Flag;

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif

static Flag flags[FLAGS_CAP];
static size_t flags_count = 0;

void *flag_new(Flag_Type type, const char *name, const char *desc)
{
    assert(flags_count < FLAGS_CAP);
    Flag *flag = &flags[flags_count++];
    flag->type = type;
    flag->name = name;
    flag->desc = desc;
    return &flag->data;
}

#endif // FLAG_IMPLEMENTATION_

//uint64_t *size = flag_uint64("name", "description");
//*size = 1337 //Default