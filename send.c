#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>

#define INPUT_MAX_SIZE 100

int fsend(int fd, char* string) {
	int length = strlen(string);
	string[length] = '\r';

	write(fd, string, length + 1);
//	int i;
//	for ( i = 0; i <= length; i++ ) {
//		printf("%c - 0x%02X\n", string[i], string[i]);
//	}
}

int main(int argc, char** argv) {
	int c = getopt (argc, argv, "d:");
	if ( c != 'd' ) {
		printf("improper usage\n");
		return -2;
	}

	int serial_fd;
	struct termios scorbot_config;
	char input[INPUT_MAX_SIZE];

	const char *device = optarg;
	serial_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

	if ( serial_fd == -1 ) {
		fprintf(stdout, "failed to open port\n");
		return -1;
	}
	tcgetattr(serial_fd, &scorbot_config);

	cfsetispeed(&scorbot_config, B9600); //set baud input to 9600
	cfsetospeed(&scorbot_config, B9600); //set baud output to 9600

	scorbot_config.c_cc[VMIN]  = 1;
	scorbot_config.c_cc[VTIME] = 0;

	scorbot_config.c_iflag = (INPCK | PARMRK); //0x18 on OS X
	scorbot_config.c_oflag = 0;
	scorbot_config.c_cflag = (CLOCAL | CREAD | CSTOPB | CS8); //0x8F00 on OS X
	scorbot_config.c_lflag = 0;

	tcsetattr(serial_fd, TCSAFLUSH, &scorbot_config);

	int status = TIOCM_DTR; //0x2 on OS X
	ioctl(serial_fd, TIOCMSET, &status);

	int length;
	if ( argc > 3 ) {
		length = strlen(argv[3]);
		fsend(serial_fd, argv[3]);
	}
	else {
		while ( fgets(input, INPUT_MAX_SIZE, stdin) != NULL ) {
			length = strlen(input);
			input[length - 1] = '\0';
			fsend(serial_fd, input);
		}
	}

	close(serial_fd);

	return 0;
}
