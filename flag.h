//The Header Part
#ifndef FLAG_H_
#define FLAG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>

typedef enum
{
    FLAG_BOOL,
    FLAG_UINT64,
    FLAG_STR,
} Flag_Type;

bool *flag_bool(const char *name, bool def, const char *desc);
uint64_t *flag_uint64(const char *name, uint64_t def, const char *desc);
char **flag_str(const char *name, char *def, const char *desc);
void flag_parse(int ac, char **av);
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
    uintptr_t def;
} Flag;

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif

#ifndef FLAGS_TMP_STR_CAP
#define FLAGS_TMP_STR_CAP 1024
#endif

static Flag flags[FLAGS_CAP];
static size_t flags_count = 0;

static char flags_tmp_str[FLAGS_TMP_STR_CAP];
static size_t flags_tmp_str_size = 0;

Flag *flag_new(Flag_Type type, const char *name, const char *desc)
{
    assert(flags_count < FLAGS_CAP);
    Flag *flag = &flags[flags_count++];
    flag->type = type;
    flag->name = name;
    flag->desc = desc;
    return flag;
}

bool *flag_bool(const char *name, bool def, const char *desc)
{
    Flag *flag = flag_new(FLAG_BOOL, name, desc);
    *((bool *)&flag->def) = def;
    *((bool *)&flag->data) = def;
    return (bool *)&flag->data;
}

uint64_t *flag_uint64(const char *name, uint64_t def, const char *desc)
{
    Flag *flag = flag_new(FLAG_UINT64, name, desc);
    *((uint64_t *)&flag->def) = def;
    *((uint64_t *)&flag->data) = def;
    return (uint64_t *)&flag->data;
}

char **flag_str(const char *name, char *def, const char *desc)
{
    Flag *flag = flag_new(FLAG_STR, name, desc);
    *((char **)&flag->def) = def;
    *((char **)&flag->data) = def;
    return (char **)&flag->data;
}

static char *flag_shift_args(int *ac, char ***av)
{
    assert(*ac > 0);
    char *result = **av;
    *av += 1;
    *ac -= 1;
    return result;
}

void flag_parse(int ac, char **av)
{
    char *program = flag_shift_args(&ac, &av);
    while (ac > 0)
    {
        char *flag = flag_shift_args(&ac, &av);
        if (*flag != '-')
        {
            fprintf(stderr, "ERROR: -%s: unkown flag\n", flag);
            exit(1);
        }
        flag++;
        size_t i = 0;
        while (i < flags_count)
        {
            if (strcmp(flags[i].name, flag) == 0)
            {
                switch (flags[i].type)
                {
                case FLAG_BOOL:
                {
                    *(bool *)&flags[i].data = true;
                }
                break;
                case FLAG_STR:
                {
                    if (ac == 0)
                    {
                        fprintf(stderr, "ERROR: -%s: no argument provided\n", flag);
                        exit(1);
                    }
                    char *arg = flag_shift_args(&ac, &av);
                    *(char **)&flags[i].data = arg;
                }
                break;
                case FLAG_UINT64:
                {
                    if (ac == 0)
                    {
                        fprintf(stderr, "ERROR: -%s: no argument provided\n", flag);
                        exit(1);
                    }
                    char *arg = flag_shift_args(&ac, &av);
                    char *endptr;
                    unsigned long long int result = strtoull(arg, &endptr, 10);
                    if (arg == endptr || *endptr != '\0')
                    {
                        fprintf(stderr, "ERROR: -%s: not a valid number\n", flag);
                        exit(1);
                    }
                    if (result == ULLONG_MAX && errno == ERANGE)
                    {
                        fprintf(stderr, "ERROR: -%s: 64 bit unsigned integer overflow\n", flag);
                        exit(0);
                    }
                    *(uint64_t *)&flags[i].data = result;
                }
                break;
                default:
                    assert(0 && "unreachable");
                    exit(69);
                }
            }
            i++;
        }
    }
    (void)program;
}

static char *flag_show_data(Flag_Type type, uintptr_t data)
{
    switch (type)
    {
    case FLAG_BOOL:
        return (*(bool *)&data) ? "true" : "false";
    case FLAG_UINT64:
    {
        int n = snprintf(NULL, 0, "%" PRIu64, *(uint64_t *)&data);
        assert(n >= 0);
        assert(flags_tmp_str_size + n + 1 <= FLAGS_TMP_STR_CAP);
        int m = snprintf(flags_tmp_str + flags_tmp_str_size,
                         FLAGS_TMP_STR_CAP - flags_tmp_str_size,
                         "%" PRIu64,
                         *(uint64_t *)&data);
        assert(n == m);
        char *result = flags_tmp_str;
        flags_tmp_str_size += n + 1;
        return result;
    }
    break;
    case FLAG_STR:
        return *(char **)&data;
    }
    assert(0 && "unreachable");
    exit(1);
}

void flag_print_help(FILE *stream)
{
    for (size_t i = 0; i < flags_count; i++)
    {
        fprintf(stream, "    -%s\n", flags[i].name);
        fprintf(stream, "        %s.\n", flags[i].desc);
        fprintf(stream, "        Default: %s\n", flag_show_data(flags[i].type, flags[i].def));
    }
}

#endif // FLAG_IMPLEMENTATION_