/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#include <core/arguments.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void print_version()
{
    printf("Binary signature enhanced finder (bsef) version 0.1\n"
           "Copyright (c) 2022 Liav A\n");
}

void print_help()
{
    printf("Usage: bsef [OPTION...] SEARCH_PARAM FILE_PATH\n"
           "Advanced options:\n"
           "\n"
           "  -a, --alignment-bytes-count   Byte address to stop look into within the file\n"
           "  -e, --end-byte-address     Byte address to stop look into within the file\n"
           "  -E, --encoding             Byte string encoding [raw-hex, ascii]\n"
           "  -n, --endianness           Endianness [little, big, middle] \n"
           "  -s, --start-byte-address   Byte address to start look into within the file\n"
           "  -?, --help                 Give this help list\n"
           "  -V, --version              Print program version\n");
}

struct state_argument const* find_option_in_list(struct state_argument const* list, char* s)
{
    size_t index = 0;
    while (list[index].name != NULL && list[index].mode != 0) {
        if (strcmp(s, list[index].name) == 0)
            return &list[index];
        index++;
    }
    return NULL;
}

static struct state_argument const endianness_options[] = {
    { "little", ENDIANNESS_LITTLE },
    { "big", ENDIANNESS_BIG },
    { "middle", ENDIANNESS_MIDDLE },
    { NULL, 0 },
};

static struct state_argument const encoding_options[] = {
    { "raw-hex", ENCODING_RAW },
    { "ascii", ENCODING_ASCII },
    { NULL, 0 },
};

struct state_argument const* default_encoding_option()
{
    return &encoding_options[0];
}

struct state_argument const* default_endianness_option()
{
    return &endianness_options[0];
}

bool parse_unsigned_long_decimal(char const* s, uint64_t* result)
{
    char* endptr = NULL;
    *result = strtol(s, &endptr, 10);
    return endptr == (s + strlen(s));
}

int arguments_parse(int argc, char** argv, struct arguments* args)
{
    int c;
    while (1) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "?", no_argument, 0, '?' },
            { "endianness", required_argument, 0, 'n' },
            { "encoding", required_argument, 0, 'E' },
            { "start-byte-address", required_argument, 0, 's' },
            { "end-byte-address", required_argument, 0, 'e' },
            { "alignment-bytes-count", required_argument, 0, 'a' },
            { "version", no_argument, 0, 'V' },
            { 0, 0, 0, 0 }
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "h?Vn:E:s:e:a:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {

        case 'h':
        case '?':
            return 2;
            break;

        case 'n': {
            struct state_argument* arg = (struct state_argument*)find_option_in_list(endianness_options, optarg);
            if (!arg) {
                printf("bsef: invalid endianness being specified.\n");
                return 1;
            }
            args->endianness = arg;
            break;
        }
        case 'E': {
            struct state_argument* arg = (struct state_argument*)find_option_in_list(encoding_options, optarg);
            if (!arg) {
                printf("bsef: invalid encoding being specified.\n");
                return 1;
            }
            args->encoding = arg;
            break;
        }

        case 's': {
            uint64_t address = 0;
            if (!parse_unsigned_long_decimal(optarg, &address)) {
                printf("bsef: invalid number for start-byte-address\n");
                return 1;
            }
            args->start_byte_address = address;
            break;
        }

        case 'e': {
            uint64_t address = 0;
            if (!parse_unsigned_long_decimal(optarg, &address)) {
                printf("bsef: invalid number for end-byte-address\n");
                return 1;
            }
            args->end_byte_address = address;
            break;
        }

        case 'a':
            break;

        case 'V':
            print_version();
            return 1;

        default:
            return 1;
        }
    }

    /* Parse any remaining command line arguments (not options). */
    if (optind < argc) {
        size_t index = 0;
        while (optind < argc) {
            switch (index) {
            case 0:
                args->search_param = argv[optind++];
                break;
            case 1:
                args->file = argv[optind++];
                break;
            default:
                printf("bsef: invalid count of positional arguments\n");
                return 2;
            }
            index++;
        }
    }
    return 0;
}
