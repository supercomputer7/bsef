/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <core/file.h>
#include <sys/types.h>

struct signature {
    size_t bytes_count;
    uint8_t* bytes;
};

struct signature* create_signature(int encoding_mode, int endianness_mode, char* search_param);
int find_signature(struct file_mapping* mapping, struct signature* sig, uint64_t start_byte_address, uint64_t end_byte_address, uint64_t alignment);
