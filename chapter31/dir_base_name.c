#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#define MAX_LENGTH 256

static pthread_once_t once_dir = PTHREAD_ONCE_INIT;
static pthread_once_t once_base = PTHREAD_ONCE_INIT;
static pthread_key_t key_dir, key_base;

static void destructor(void *buf) {
    free(buf);
}

static void create_key_dir() {
    if (pthread_key_create(&key_dir, destructor) != 0) {
        exit(EXIT_FAILURE);
    }
}

static void create_key_base() {
    if (pthread_key_create(&key_base, destructor) != 0) {
        exit(EXIT_FAILURE);
    }
}

char *dirname_safe(char *path) {
    int s;
    char *buf;

    s = pthread_once(&once_dir, create_key_dir);
    buf = (char *) pthread_getspecific(key_dir);

    if (buf == NULL) {
        if ((buf = (char *) malloc(MAX_LENGTH)) == NULL) {
            exit(EXIT_FAILURE);
        }

        if ((s = pthread_setspecific(key_dir, buf)) != 0) {
            exit(EXIT_FAILURE);
        }
    }

    char *pos = strrchr(path, '/');

    if (pos == NULL) {
        buf[0] = '.';
        buf[1] = '\0';
    } else if (pos == path) {
        buf[0] = '/';
        buf[1] = '\0';
    } else {
        strncpy(buf, path, pos - path);
        buf[pos - path] = '\0';
    }
    
    return buf;
}

static void *func(void *arg) {
    char s[] = "/boot/grub";
    return dirname_safe(s);
}

int main(int argc, char *argv[]) {
    char s[] = "/bin/bash";
    pthread_t tid;
    void *r1, *r2;

    r1 = dirname_safe(s);

    assert(pthread_create(&tid, NULL, func, NULL) == 0);
    assert(pthread_join(tid, &r2) == 0);
    assert(strcmp((char *) r1, "/bin") == 0);
    assert(strcmp((char *) r2, "/boot") == 0);

    printf("Exercise31-2 succeed!\n");

    exit(EXIT_SUCCESS);
}
