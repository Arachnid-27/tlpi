#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <pwd.h>

struct passwd *getpwnam2(const char *name) {
    struct passwd *pwd;

    while ((pwd = getpwent()) != NULL) {
        if (strcmp(pwd->pw_name, name) == 0) break;
    }
    endpwent();

    return pwd;
}

int main(int argc, char *argv[]) {
    assert(getpwnam2("hiroshi")->pw_uid == 1000);
    assert(getpwnam2("root")->pw_uid == 0);
    assert(getpwnam2("return_null") == NULL);
    printf("Exercise8-2 succeed!\n");
    exit(EXIT_SUCCESS);
}
