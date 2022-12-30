/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#include <core/arguments.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    struct arguments arguments;
    /* Default values. */
    arguments.start_byte_address = 0;
    arguments.end_byte_address = 0;
    arguments.alignment_bytes_count = 0;
    arguments.encoding = (struct state_argument*)default_encoding_option();
    arguments.search_param = NULL;
    arguments.verbose = false;
    arguments.endianness = (struct state_argument*)default_endianness_option();
    arguments.file = "-";

    int rc = arguments_parse(argc, argv, &arguments);
    if (rc == 1)
        exit(1);
    if (rc == 2 || arguments.search_param == NULL) {
        print_help();
        exit(0);
    }

    exit(0);
}
