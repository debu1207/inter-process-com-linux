#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "smem.h"

void dump_exit(const char* msg) {
	perror(msg);
	exit(-1);
}

int main() {
	int fd = shm_open(BackingFile,
			  O_RDWR | O_CREAT,
			  AccessPerms);

	if (fd <0)
		dump_exit("Can't open the shared mem segment...");

	ftruncate(fd, ByteSize); /* get the bytes */

	caddr_t memptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if ((caddr_t) -1 == memptr)
		dump_exit("Can't get segment...");

	fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize -1 );
	fprintf(stderr, "backing file:     /dev/shm%s\n", BackingFile);

	/* Semaphore code to lock the shared mem */
	sem_t* semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);

	if (semptr == (void*) -1)
		dump_exit("Error in sem_open\n");

	strcpy(memptr, MemContents); /*copy some ASCII bytes to the segment */

	/* increment the semaphore so that memreader can read*/

	if (sem_post(semptr) <0)
		dump_exit("Sem_post\n");

	sleep(12);

	munmap(memptr, ByteSize);
	close(fd);
	sem_close(semptr);
	shm_unlink(BackingFile);
	return 0;
}
