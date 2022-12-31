/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: MIT
 */

#include <core/file.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int create_fd(char* file_path)
{
    if (strcmp(file_path, "-") == 0)
        return STDIN_FILENO;
    return open(file_path, O_RDONLY);
}

struct file_mapping* create_file_backed_memory_region(char* file_path, size_t mapping_size)
{
    long page_size = sysconf(_SC_PAGE_SIZE);
    void* buf = aligned_alloc(page_size, mapping_size);
    if (!buf) {
        printf("bsef: fail to create file mapping with buffer sized %u.\n", mapping_size);
        return NULL;
    }   
    int rc = create_fd(file_path);
    if (rc < 0) {
        perror("bsef: fail to create file mapping");
        return NULL;
    }
    size_t file_size = 0;
    if (lseek(rc, 0, SEEK_SET) == -1) {
        file_size = UINT64_MAX;
    } else {
        struct stat st;
        memset(&st, 0, sizeof(struct stat));
        if (fstat(rc, &st) < 0) {
            free(buf);
            close(rc);
            printf("bsef: fail to create file mapping due fstat failure.\n");
            return NULL;
        }
        file_size = st.st_size;
    }
    struct file_mapping* mapping = (struct file_mapping*)malloc(sizeof(struct file_mapping));
    memset(mapping, 0, sizeof(struct file_mapping));
    mapping->fd = rc;
    mapping->buf = buf;
    mapping->buf_size = mapping_size;
    mapping->file_size = file_size;
    return mapping;
}

ssize_t update_file_backed_memory_region_peek_offset(struct file_mapping* mapping, size_t new_mapping_offset)
{
    memset(mapping->buf, 0, mapping->buf_size);
    off_t rc = lseek(mapping->fd, new_mapping_offset, SEEK_SET);
    if (rc < 0)
        return -errno;
    return read(mapping->fd, mapping->buf, mapping->buf_size);
}

void clean_file_mapping(struct file_mapping* mapping)
{
    free(mapping->buf);
    close(mapping->fd);
    free(mapping);
}
