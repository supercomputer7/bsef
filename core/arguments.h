/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct state_argument {
    char const* name;
    uint64_t mode;
};

#define ENDIANNESS_LITTLE 1
#define ENDIANNESS_BIG 2
#define ENDIANNESS_MIDDLE 3

#define ENCODING_RAW 1
#define ENCODING_ASCII 2

struct arguments {
    uint64_t start_byte_address;
    uint64_t end_byte_address;
    uint64_t alignment_bytes_count;
    char* search_param;
    struct state_argument* encoding;
    struct state_argument* endianness;
    char* file;
    bool verbose;
};

struct state_argument const* default_encoding_option();
struct state_argument const* default_endianness_option();

int arguments_parse(int argc, char** argv, struct arguments* args);
void print_help();
