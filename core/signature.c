/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#include <core/arguments.h>
#include <core/signature.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool validate_hex_character_to_byte(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    } else if (ch >= 'A' && ch <= 'F') {
        return true;
    } else if (ch >= 'a' && ch <= 'f') {
        return true;
    }
    return false;
}

uint8_t decode_2_hex_characters_to_byte(char ch1, char ch2)
{
    uint8_t result = 0;

    if (ch2 >= '0' && ch2 <= '9') {
        result |= (ch2 - 48);
    } else if (ch2 >= 'A' && ch2 <= 'F') {
        result |= (ch2 - 55);
    } else if (ch2 >= 'a' && ch2 <= 'f') {
        result |= (ch2 - 87);
    }

    if (ch1 >= '0' && ch1 <= '9') {
        result |= (ch1 - 48) << 4;
    } else if (ch1 >= 'A' && ch1 <= 'F') {
        result |= (ch1 - 55) << 4;
    } else if (ch1 >= 'a' && ch1 <= 'f') {
        result |= (ch1 - 87) << 4;
    }

    return result;
}

struct signature* create_signature(int encoding_mode, int endianness_mode, char* search_param)
{
    void* sig_pattern = NULL;
    size_t search_pattern_length = 0;
    size_t search_param_length = strlen(search_param);
    if (encoding_mode == ENCODING_RAW) {
        if ((search_param_length % 2) != 0) {
            printf("bsef: invalid raw hex signature length.\n");
            return NULL;
        }
        if (endianness_mode == ENDIANNESS_MIDDLE && (search_param_length % sizeof(uint32_t)) != 0) {
            printf("bsef: invalid raw hex signature length when using middle endian mode.\n");
            return NULL;
        }

        // NOTE: Each hex digit is 4 bits (half a byte).
        sig_pattern = malloc(search_param_length / 2);

        // NOTE: Don't include the null terminator
        search_pattern_length = search_param_length / 2;
        if (endianness_mode == ENDIANNESS_BIG) {
            for (size_t index = 0; index < search_param_length; index += 2) {
                if (!validate_hex_character_to_byte(search_param[index]) || !validate_hex_character_to_byte(search_param[index + 1])) {
                    printf("bsef: invalid raw hex character signature.\n");
                    return NULL;
                }
                uint8_t byte = decode_2_hex_characters_to_byte(search_param[index], search_param[index + 1]);
                ((uint8_t*)sig_pattern)[index / 2] = byte;
            }
        } else if (endianness_mode == ENDIANNESS_LITTLE) {
            for (size_t index = 0; index < search_param_length; index += 2) {
                if (!validate_hex_character_to_byte(search_param[search_param_length - index - 2]) || !validate_hex_character_to_byte(search_param[search_param_length - index - 1])) {
                    printf("bsef: invalid raw hex character signature.\n");
                    return NULL;
                }
                uint8_t byte = decode_2_hex_characters_to_byte(search_param[search_param_length - index - 2], search_param[search_param_length - index - 1]);
                ((uint8_t*)sig_pattern)[index / 2] = byte;
            }
        } else if (endianness_mode == ENDIANNESS_MIDDLE) {
            size_t byte_index = 0;
            for (size_t index = 0; index < search_param_length; index += 2) {
                if (!validate_hex_character_to_byte(search_param[index]) || !validate_hex_character_to_byte(search_param[index + 1])) {
                    printf("bsef: invalid raw hex character signature.\n");
                    return NULL;
                }
                uint8_t byte = decode_2_hex_characters_to_byte(search_param[index], search_param[index + 1]);

                if (byte_index % 2 == 0) {
                    ((uint8_t*)sig_pattern)[byte_index + 1] = byte;
                } else {
                    ((uint8_t*)sig_pattern)[byte_index - 1] = byte;
                }
                byte_index++;
            }
        }
    } else if (encoding_mode == ENCODING_ASCII) {
        sig_pattern = malloc(sizeof(uint32_t));
        memcpy(sig_pattern, search_param, search_param_length);
        search_pattern_length = search_param_length;
    }
    struct signature* sig = (struct signature*)malloc(sizeof(struct signature));
    sig->bytes_count = search_pattern_length;
    sig->bytes = sig_pattern;
    return sig;
}

bool compare_signature(uint8_t* buf, struct signature* sig)
{
    for (size_t index = 0; index < sig->bytes_count; index++) {
        if (sig->bytes[index] != buf[index]) {
            return false;
        }
    }
    return true;
}

int find_signature(struct file_mapping* mapping, struct signature* sig, uint64_t start_byte_address, uint64_t end_byte_address, uint64_t alignment)
{
    uint64_t current_byte_address = start_byte_address;
    if (start_byte_address != 0)
        current_byte_address += alignment % start_byte_address;
    while (current_byte_address < end_byte_address) {
        ssize_t rc = update_file_backed_memory_region_peek_offset(mapping, current_byte_address);
        if (rc < 0)
            return 1;
        if (rc == 0)
            return 0;
        size_t incrementer = 1;
        if (alignment > 0)
            incrementer = alignment;
        for (size_t byte_index = 0; byte_index < fmin(end_byte_address - current_byte_address, mapping->buf_size - sig->bytes_count); byte_index += incrementer) {
            if (compare_signature(&mapping->buf[byte_index], sig)) {
                printf("byte address %u: found signature\n", current_byte_address + byte_index);
                byte_index += sig->bytes_count;
            }
        }
        current_byte_address += mapping->buf_size;
    }
    return 0;
}
