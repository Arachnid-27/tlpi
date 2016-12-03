#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int setenv2(const char *name, const char *value, int overwrite) {
    if (getenv(name) == NULL || overwrite) {
        size_t len1 = strlen(name);
        size_t len2 = strlen(value);
        char *buf = (char*) malloc(len1 + len2 + 2);
        memmove(buf, name, len1);
        *(buf + len1) = '=';
        memmove(buf + len1 + 1, value, len2);
        *(buf + len1 + len2 + 1) = 0;
        return !putenv(buf) ? 0 : -1;
    }
    return 0;
}

int main() {
    assert(!setenv2("TEST", "one", 0));
    assert(!strcmp(getenv("TEST"), "one"));
    assert(!setenv2("TEST", "two", 0));
    assert(!strcmp(getenv("TEST"), "one"));
    assert(!setenv2("TEST", "two", 1));
    assert(!strcmp(getenv("TEST"), "two"));
    printf("Exercise6-3 setenv succeed!\n");
    exit(EXIT_SUCCESS);
}
