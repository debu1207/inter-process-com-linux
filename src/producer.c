#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILENAME "data.dump"
#define DATA "A randome dump string to put in the data.dump file\n Interesting to learn inter process # communication"

void dump_exit(const char* msg) {
	perror(msg);
	exit(-1); /*EXIT_FAILURE*/
}

int main() {
	struct flock lock;
	lock.l_type = F_WRLCK; /* read/write exclusive lock (not shared)*/
	/* If any producer gains the lock, then no other process will be able 
	 * to write or read the file until the producer releases the lock, either
	 * explicitly with the appropriate call to fcntl or implicitly by closing the file
	 */

	lock.l_whence = SEEK_SET; /* base for seek offsets */
	lock.l_start = 0; /* 1st byte infile */
//	lock.l_end = 0; /* 0 here meas 'until EOF' 
	lock.l_pid = getpid(); /* process id */

	int fd; /* file descriptor to identify a file */

	fd = open(FILENAME, O_RDWR | O_CREAT, 0666);

	if (fd < 0)
		dump_exit("Open failed...");

	/* F_SETLK, F_SETLKW and F_GETLK are used to acquire, release, and test for the 
	 * existence of record locks.
	 * If a conflicting lock is held by another process, this call returns -1 and sets
	 * errno to EACCESS or EAGAIN.
	 */

	if (fcntl(fd, F_SETLK, &lock) < 0) /* F_SETLK doesn't block, F_SETLKW does */
		dump_exit("fcntl failed to get lock...");
	else {
		write(fd, DATA, strlen(DATA)); /* Populate data file */
		fprintf(stderr, "Process %d has written to data file ...\n", lock.l_pid);
	}

	/* Now release the lock explicitly */
	lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		dump_exit("Explicit unlocking failed...");

	close(fd); /* Close the file: would unlock if needed */

	return 0;
}
