#include <unistd.h>
#include <error.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//#define BUFSIZE 4096 //4K
//#define BUFSIZE 262144 //256K
//#define BUFSIZE 1048576 //1M

//#define BUFSIZE 10485760 //10M
//#define BUFSIZE 52428800 //50M
#define BUFSIZE 104857600 // 100M

int compute(char *buf, int read_bytes)
{
    int i;

    for (i=0; i<read_bytes; i++)
    {
        //printf("%c", buf[i]);
    }

    //printf("\n");
    return i;
}

int main(int argc, char *argv[])
{
    char *app_reader = "/tmp/app_reader";
    char *app_writer = "/tmp/app_writer";

    int read_fd, write_fd;
    //char buf[BUFSIZE] = {'\n'};
    char *buf;
    buf = (char*) malloc(BUFSIZE*sizeof(char));
    int count = 0;

    if (!access(app_writer, F_OK) == 0)
    {
        mkfifo(app_writer, 0666);
    }

    if (!access(app_reader, F_OK) == 0)
    {
        mkfifo(app_reader, 0666);
    }

    //printf("Pipes ready\n");

    read_fd = open(app_writer, O_RDONLY);
    //printf("File open to be read: %d\n", read_fd);
    if (read_fd < 0)
    {
        printf("%s:Could not open %s for reading %s\n", argv[0], app_writer, strerror(errno));
        exit(1);
    }

    //printf("Start Read\n");

    int read_bytes = 0;
    clock_t tic = clock();
    read_bytes = read(read_fd, buf, BUFSIZE);
    while (read_bytes > 0)
    {
        //printf("read bytes = %d\n", read_bytes);
        buf[read_bytes] = '\0';
        count += compute(buf, read_bytes);
        //printf("Read Line Complete, count %d\n", count);
        read_bytes = read(read_fd, buf, BUFSIZE);
    }

    clock_t toc = clock();

    //printf("All Reads Complete\n");
    close(read_fd);
/*
    if (errno)
    {
        printf("%s:Could not read %s\n", argv[0], strerror(errno));
        exit(1);
    }
*/

    write_fd = open(app_reader, O_WRONLY);
    //printf("file open to be written: %d\n", write_fd);
    if (write_fd < 0)
    {
        printf("%s:Could not open %s for writing %s\n", argv[0], app_reader, strerror(errno));
        exit(1);
    }

    //printf("Writing Back Count = %d\n", count);
    int result = write(write_fd, &count, sizeof(int));

    if (result < 0)
    {
        printf("%s:Could not write %s\n", argv[0], strerror(errno));
        exit(1);
    }

    //printf("done\n");
    close(write_fd);

    printf("Elapsed time in processing(ms): %f\n", ((double)(toc - tic)*(1000)) / CLOCKS_PER_SEC);

    exit(0);
}
