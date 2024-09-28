#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "data.dump"

void dump_exit(const char* msg) {
	perror(msg);
	exit(-1);
}

int main() {
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	int fd;

	if ((fd = open(FILENAME, O_RDONLY)) < 0)
		dump_exit("Open to read failed...");

	/* If the file is write-locked, we can't continue */
	fcntl(fd, F_GETLK, &lock);
	if (lock.l_type != F_UNLCK)
		dump_exit("File is still write locked...");

	lock.l_type = F_RDLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		dump_exit("Can't get a read-only lock...");

	/* Read the bytes one at a time */
	int c; /* buffer for read bytes */

	while (read(fd, &c, 1) > 0)
		write(STDOUT_FILENO, &c, 1);

	lock.l_type = F_UNLCK;

	if (fcntl(fd, F_SETLK, &lock) <0)
		dump_exit("Explicit unlocking failed...");

	close(fd);
	return 0;

}
