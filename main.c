#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>

#define INPUT_MAX_SIZE 100

int main() {
	int pid;
	int serial_fd;
	struct termios scorbot_config;
	char input[INPUT_MAX_SIZE];
	char c;

	const char *device = "/dev/tty.usbserial-A900IKU8";
	serial_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

	if ( serial_fd == -1 ) {
		fprintf(stdout, "failed to open port\n");
	}
	else {
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
	}

	initscr();
	cbreak();
	noecho();

	pid = fork(); //splits process for recieving and sending

	if ( pid == 0 ) { //child process continually receives
		if ( serial_fd == -1 ) {
			return -1;
		}

		while ( 1 ) {
			if ( read(serial_fd, &c, 1) > 0 ) {
				printw("%c", c);
			}
		}
	}
	else { //parent process sends input on return
		fprintf(stdout, "sending process started\n"); //-debug
		int length;
		while ( 1 ) {
			c = getch();
			if ( c == '\04' ) {
				break;
			}
			if ( c == '\n' ) {
				c = '\r'; // replace newline with carriage return
			}
			printw("typed: %c - 0x%02x\n\r", c, c);
			if ( serial_fd != -1 ) {
			  write(serial_fd, input, length);
			}
//			fprintf(stdout, "sending:\n");
//			int i;
//			for ( i = 0; i < length; i++ ) {
//				fprintf(stdout, "%c - %02x\n", input[i], input[i]);
//			}
		}
		kill(pid);
		wait(pid);
	}

	close(serial_fd);

	fprintf(stdout, "complete\n");

	return 0;
}
