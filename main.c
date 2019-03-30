#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pcre.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void load_file_to_RAM(char **buffer, const char *filename) { 
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        goto error;
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        fprintf(stderr, "fstat error\n");
        goto error;
    }

    void *src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (src == MAP_FAILED) {
        fprintf(stderr, "Calling error of mmap\n");
        goto error;
    }

    *buffer = (char*) calloc(sizeof(char), (size_t) statbuf.st_size + 1);
    if (!(*buffer)) {
        fprintf(stderr, "Cannot allocate memory\n");
        free(*buffer);
        goto error;
    }
    memcpy(*buffer, src, (size_t) statbuf.st_size);   
    munmap(src, statbuf.st_size);
    return;

error:
    close(fd);
    *buffer = NULL;
    return;
}

int search_substring(const char *pattern, const char *buffer,
                      char **substring) {
    pcre *pcre_comp = NULL;
    const char *pcre_error_str = NULL;
    int pcre_error_offset = 0;
    pcre_comp = pcre_compile(pattern, 0, &pcre_error_str, &pcre_error_offset, NULL);
    if (pcre_comp == NULL) {
        printf("ERROR: Could not compile '%s': %s\n", pattern, pcre_error_str);
        return 0;
    }

    pcre_extra *pcre_extr = pcre_study(pcre_comp, 0, &pcre_error_str);
    if (pcre_error_str != NULL) {
        printf("ERROR: Could not study '%s': %s\n", pattern, pcre_error_str);
        return 0;
    }

    const int ovecsize = 2;
    int ovector[2] = {0};
    int string_count = pcre_exec(pcre_comp, pcre_extr, buffer, strlen(buffer), 0, 0, ovector, ovecsize);
    if (string_count < 0) {
        fprintf(stderr, "ERROR: The coincidence was not found out\n");
        *substring = NULL;
        return 0;
    }
    size_t substring_size = (size_t) (ovector[1] - ovector[0]);
    *substring = (char*) calloc(sizeof(char), substring_size + 1);
    if (!(*substring)) {
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        free(*substring);
        return 0;
    }
    memcpy(*substring, buffer + ovector[0], substring_size);

    pcre_free(pcre_comp);
    pcre_free(pcre_extr);

    return string_count;
}

void take_patterns(const char *filename) {
    
    return;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: Command must contain: %s <file_name>\n", argv[0]);
        return 1;
    }

    char pattern_from[] = "Date: [a-zA-z]{3}[,]? [0-9]{2} [a-zA-Z]{3} [0-9]{4} ([0-9]{2}[:]?)+ [+][0-9]{4}";

    char *buffer;
    load_file_to_RAM(&buffer, argv[1]);
    if (!buffer) {
        fprintf(stderr, "ERROR: Buffer is NULL after loading from %s\n", argv[1]);
        return 1;
    }
    
    char *substring = NULL;
    printf("%d\n", search_substring(pattern_from, buffer, &substring));
    if (!substring) {
        fprintf(stderr, "ERROR:\n");
        free(substring);
        free(buffer);

        return 1;
    } 
    printf(substring);

    free(substring);
    free(buffer);

    return 0;
}
