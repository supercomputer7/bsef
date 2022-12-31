/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#include <core/arguments.h>
#include <core/file.h>
#include <core/signature.h>
#include <stdio.h>
#include <stdlib.h>

bool validate_byte_offset_arguments(uint64_t file_size, uint64_t start_byte_address, uint64_t end_byte_address, uint64_t alignment)
{
    if ((alignment % 2) != 0) {
        printf("bsef: invalid alignment being specified.");
        return false;
    }

    if (end_byte_address == 0)
        end_byte_address = file_size;
    if (start_byte_address > end_byte_address) {
        printf("bsef: end byte address being specified is before start byte address.");
        return false;
    }

    if ((end_byte_address - start_byte_address) < alignment) {
        printf("bsef: alignment being specified is smaller than possible read size.");
        return false;
    }
    return true;
}

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

    struct signature* sig = create_signature(arguments.encoding->mode, arguments.endianness->mode, arguments.search_param);
    if (!sig) {
        exit(1);
    }
    if (sig->bytes_count > 32768) {
        printf("bsef: bytes signature invalid size.\n");
        exit(1);
    }

    struct file_mapping* mapping = create_file_backed_memory_region(arguments.file, 32768);
    if (!mapping) {
        exit(1);
    }

    if (!validate_byte_offset_arguments(mapping->file_size, arguments.start_byte_address, arguments.end_byte_address, arguments.alignment_bytes_count))
        exit(1);

    if (arguments.end_byte_address == 0)
        arguments.end_byte_address = mapping->file_size;

    rc = find_signature(mapping, sig, arguments.start_byte_address, arguments.end_byte_address, arguments.alignment_bytes_count);
    free(sig->bytes);
    clean_file_mapping(mapping);
    exit(rc);
}
