#include <signal.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

static void setup(mqd_t *mqdp);

static void handler(union sigval sv) {
    char buf[1024];
    mqd_t *mqdp;

    mqdp = (mqd_t *) (sv.sival_ptr);

    setup(mqdp);

    while (mq_receive(*mqdp, buf, 1024, NULL) > 0) {
        printf("%s", buf);
    }

    if (errno != EAGAIN) {
        errExit("mq_receive");
    }
}

static void setup(mqd_t *mqdp) {
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = handler;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqdp;

    if (mq_notify(*mqdp, &sev) == -1) {
        errExit("mq_notify");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s mq-name\n", argv[0]);
    }

    mqd_t mqd1, mqd2;
    char buf[1024];
    char msg[1024];
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;

    if ((mqd1 = mq_open(argv[1], O_RDONLY | O_NONBLOCK | O_CREAT, S_IRUSR | S_IWUSR, &attr)) == -1) {
        errExit("mq_open %s", argv[1]);
    }

    printf("receiver: ");
    fgets(buf, 1024, stdin);
    
    char *s = strchr(buf, '\n');
    if (s != NULL) {
        *s = 0;
    }

    if ((mqd2 = mq_open(buf, O_WRONLY)) == -1) {
        errExit("mq_open %s", buf);
    }
    
    setup(&mqd1);

    while (1) {
        if (fgets(buf, 1024, stdin) != NULL) {
            snprintf(msg, 1024, "(%ld): %s", (long) getpid(), buf);
            if (mq_send(mqd2, msg, strlen(msg) + 1, 0) == -1) {
                errExit("mq_send");
            }
        } else if (errno != EINTR) {
            break;
        }
    }

    mq_unlink(argv[1]);

    exit(EXIT_SUCCESS);
}
