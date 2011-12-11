#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
	int pid, serial_fd;
	struct termios config;

	const char *device = "/dev/tty.usbserial-A900IKU8";
	serial_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if ( serial_fd == -1 ) {
		fprintf(stdout, "failed to open port\n");
	}

	tcgetattr(serial_fd, &config);
	printf("%X\n", (unsigned int) config.c_iflag);
	printf("%X\n", (unsigned int) config.c_oflag);
	printf("%X\n", (unsigned int) config.c_cflag);
	printf("%X\n", (unsigned int) config.c_lflag);
	//cc_t     c_cc[NCCS]; /* special characters */

	int status;
	ioctl(serial_fd, TIOCMGET, &status);
	printf("%X\n", status);

	close(serial_fd);

	return 0;
}
