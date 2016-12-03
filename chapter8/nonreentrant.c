#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

int main(int argc, char *argv[]) {
    printf("%ld %ld\n", (long) (getpwnam("root")->pw_uid), (long) (getpwnam("hiroshi")->pw_uid));
    printf("%s %s\n", getpwuid(1)->pw_name, getpwuid(1000)->pw_name);
    exit(EXIT_SUCCESS);
}
