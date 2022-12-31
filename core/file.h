/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

struct file_mapping {
    int fd;
    uint8_t* buf;
    size_t buf_size;
    size_t file_size;
};

struct file_mapping* create_file_backed_memory_region(char* file_path, size_t mapping_size);
ssize_t update_file_backed_memory_region_peek_offset(struct file_mapping*, size_t new_mapping_offset);
void clean_file_mapping(struct file_mapping*);
