#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/uinput.h>

int setup_uinput_device()
{
    struct uinput_user_dev uidev;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        return fd;
    }

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0 ||
        ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0 ||
        ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) < 0 ||
        ioctl(fd, UI_SET_EVBIT, EV_REL) < 0 ||
        ioctl(fd, UI_SET_RELBIT, REL_X) < 0 ||
        ioctl(fd, UI_SET_RELBIT, REL_Y) < 0) {
        perror("ioctl");
        close(fd);
        return -1;
    }

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Virtual Mouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    if (write(fd, &uidev, sizeof(uidev)) < 0 ||
        ioctl(fd, UI_DEV_CREATE) < 0) {
        perror("setup");
        close(fd);
        return -1;
    }

    sleep(1);
    return fd;
}

void move_mouse(int fd, int dx, int dy)
{
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_REL;
    ev.code = REL_X;
    ev.value = dx;
    if (write(fd, &ev, sizeof(ev)) < 0) perror("write REL_X");

    ev.type = EV_REL;
    ev.code = REL_Y;
    ev.value = dy;
    if (write(fd, &ev, sizeof(ev)) < 0) perror("write REL_Y");

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(fd, &ev, sizeof(ev)) < 0) perror("write SYN_REPORT");
}

void destroy_uinput_device(int fd)
{
    if (ioctl(fd, UI_DEV_DESTROY) < 0) perror("UI_DEV_DESTROY");
        close(fd);
}
