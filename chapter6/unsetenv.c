#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int unsetenv2(const char *name) {
    size_t len = strlen(name);
    char *buf = (char*) malloc(len);
    memmove(buf, name, len);
    while (getenv(name) != NULL) {
        if (putenv(buf)) {
            return -1;
        }
    }
    return 0;
}

int main() {
    assert(!setenv("TEST", "one", 0));
    assert(!strcmp(getenv("TEST"), "one"));
    assert(!unsetenv2("TEST"));
    assert(getenv("TEST") == NULL);
    printf("Exercise6-3 unsetenv succeed!\n");
    return 0;
}
