/*
** Near Disk process for charCounter
**
** This C program will perform the computation of aggregating
** the total character count of a file when near disk compute
** driver function offloads the operation to this process.
**
** It communicates with the near disk driver function through
** read and write pipes, reading the input to be processed,
** and writing back the result after processing it.
**
** Several printf calls are commented out in general but can
** be used to debug when required.
*/

// including required libraries
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/*
** Using different buffer sizes to check if better performance
** can be achieved for processing the task with a larger buffer.
** Saves overhead of large number of read operations and thus
** more time is spent on actually computing character counts.
*/
//#define BUFSIZE 4096 //4K
//#define BUFSIZE 262144 //256K
//#define BUFSIZE 1048576 //1M
//#define BUFSIZE 10485760 //10M
//#define BUFSIZE 52428800 //50M
#define BUFSIZE 104857600 // 100M


/*
** Core function that takes the read bytes as an argument and
** computes and returns the total character count. It traverses
** through the whole read buffer to be equivalent to the host
** compute driver function which also does this traversal.
*/
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
    // input and output files used for reading and writing
    // data between near disk compute and this process
    char *app_reader = "/tmp/app_reader";
    char *app_writer = "/tmp/app_writer";

    int read_fd, write_fd, count = 0;
    char *buf;

    // allocate a huge buffer of BUFSIZE
    buf = (char*) malloc(BUFSIZE*sizeof(char));

    // Make the input and output files as special FIFO
    // pipes to facilitate reading and writing as streams
    if (!access(app_writer, F_OK) == 0)
    {
        mkfifo(app_writer, 0666);
    }

    if (!access(app_reader, F_OK) == 0)
    {
        mkfifo(app_reader, 0666);
    }

    //printf("Pipes ready\n");


    // Open read pipe for reading data
    // sent by the near disk driver function
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

    // read data from pipe untill there is data to be read
    read_bytes = read(read_fd, buf, BUFSIZE);
    while (read_bytes > 0)
    {
        //printf("read bytes = %d\n", read_bytes);
        buf[read_bytes] = '\0';

        // call compute function to compute the character count
        // and aggregate with previous count
        count += compute(buf, read_bytes);

        //printf("Read Line Complete, count %d\n", count);
        read_bytes = read(read_fd, buf, BUFSIZE);
    }

    clock_t toc = clock();

    //printf("All Reads Complete\n");

    // close the read pipe
    close(read_fd);

    // Open write pipe for writing data to be
    // sent to the near disk driver function
    write_fd = open(app_reader, O_WRONLY);
    //printf("file open to be written: %d\n", write_fd);
    if (write_fd < 0)
    {
        printf("%s:Could not open %s for writing %s\n", argv[0], app_reader, strerror(errno));
        exit(1);
    }

    //printf("Writing Back Count = %d\n", count);

    // write the aggregated character count
    // to the write pipe
    int result = write(write_fd, &count, sizeof(int));

    if (result < 0)
    {
        printf("%s:Could not write %s\n", argv[0], strerror(errno));
        exit(1);
    }

    // close the write pipe
    close(write_fd);

    // Print the elapsed time for processing charCounter
    // as an offloaded task to this process
    printf("Elapsed time in processing(ms): %f\n", ((double)(toc - tic)*(1000)) / CLOCKS_PER_SEC);

    exit(0);
}
