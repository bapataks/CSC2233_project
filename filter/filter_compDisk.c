/*
** Near Disk process for filter
**
** This C program will perform the computation of filtering a file
** to keep only the lines that satisfy the set filter predicate
** when near disk driver function offloads the operation to this
** process.
**
** It communicates with the near disk driver function through
** read and write pipes, reading the input to be processed,
** and writing back the result after processing it.
**
** Several printf calls are commented out in general but can
** be used to debug when required.
*/

// include required libraries
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// use large buffer for reading input
#define BUFSIZE 104857600 // 100M


// filter predicate used for filtering the lines of a file
bool filterString(char *str)
{
    // keep lines with length less than 20 characters
    return strlen(str) < 20;
}


/*
** Core function that takes the read bytes as an argument and
** processes the filter operation. It identifies the lines of
** the input file, checks for filter predicate, adds to result
** in case of success and discards in case of failure.
*/
void compute(char *buf, char **tmp, int *size, int read_bytes, char **out, int *outLength)
{
    int i,prev;

    for (i=0, prev=0; i<read_bytes; i++)
    {
        //printf("%c", buf[i]);

        // identify a line separator
        if (buf[i] == '\n')
        {
            // copy line contents in tmp
            // it could either be part of this buffer completely
            // or split between this buffer and earlier read buffer
            if (*size == 0)
            {
                *tmp = (char*) malloc((i-prev)*sizeof(char));
            }
            else
            {
                *tmp = (char*) realloc(*tmp, (*size+i-prev)*sizeof(char));
            }
            memcpy(*tmp + *size, buf + prev, i - prev);
            *size += i - prev;

            // check filter predicate
            if (filterString(*tmp))
            {
                //printf("filtered string:%s\nsize:%d\n", *tmp, *size);

                // add line to the output result
                if (*outLength == 0)
                {
                    *out = (char*) malloc((*size+1)*sizeof(char));
                }
                else
                {
                    *out = (char*) realloc(*out, (*outLength+*size+1)*sizeof(char));
                }
                memcpy(*out + *outLength, *tmp, *size);
                memcpy(*out + *outLength + *size, buf + i, 1);
                *outLength += *size+1;
                //printf("out string:%s\nlength:%d\n", *out, *outLength);
            }

            // free tmp pointer used to store one line
            free(*tmp);
            *size = 0;
            prev = i+1;
        }
    }

    if (*size == 0)
    {
        *tmp = (char*) malloc((i-prev)*sizeof(char));
    }
    else
    {
        *tmp = (char*) realloc(*tmp, (*size+i-prev)*sizeof(char));
    }
    memcpy(*tmp + *size, buf + prev, i - prev);
    *size += i - prev;

    /*
    printf("tmp: %s\nbuf: %s\n", *tmp, buf);
    printf("tmp addr: %p\n", (void*)*tmp);
    printf("size:%d\n", *size);
    */

    return;
}

int main(int argc, char *argv[])
{
    // input and output files used for reading and writing
    // data between near disk compute and this process
    char *app_reader = "/tmp/app_reader";
    char *app_writer = "/tmp/app_writer";

    int read_fd, write_fd, size=0, outLength=0;
    char *buf, *tmp;
    char *out;

    // allocate a huge buffer of BUFSIZE
    buf = (char*) malloc(BUFSIZE*sizeof(char));

    // make the input and output files as special FIFO
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

    // open read pipe for reading data
    // sent by the near disk driver function
    read_fd = open(app_writer, O_RDONLY);
    //printf("File open to be read: %d\n", read_fd);
    if (read_fd < 0)
    {
        printf("%s:Could not open %s for reading %s\n", argv[0], app_writer, strerror(errno));
        exit(1);
    }

    int read_bytes = 0;
    clock_t tic = clock();

    // read data from pipe untill there is data to be read
    read_bytes = read(read_fd, buf, BUFSIZE);
    while (read_bytes > 0)
    {
        buf[read_bytes] = '\0';

        // call compute function to filter the input file
        // and aggregate result in out pointer
        compute(buf, &tmp, &size, read_bytes, &out, &outLength);

        //printf("out:%s\noutLength:%d\naddr:%p\n", out, outLength, (void*)out);
        read_bytes = read(read_fd, buf, BUFSIZE);
    }

    // code the read pipe
    close(read_fd);

    //printf("Final filtered output:\n%s\n", out);

    // open write pipe for writing data to be
    // sent to the near disk driver function
    write_fd = open(app_reader, O_WRONLY);
    //printf("file open to be written: %d\n", write_fd);
    if (write_fd < 0)
    {
        printf("%s:Could not open %s for writing %s\n", argv[0], app_reader, strerror(errno));
        exit(1);
    }

    // write the aggregated result of performing filter
    // operation on the input file
    write(write_fd, &outLength, sizeof(int));
    int result = write(write_fd, out, outLength*sizeof(char));

    clock_t toc = clock();

    if (result < 0)
    {
        printf("%s:Could not write %s\n", argv[0], strerror(errno));
        exit(1);
    }

    // close the write pipe
    close(write_fd);

    // print the elapsed time for processing filter
    // as an offloaded task to this process
    printf("Elapsed time in processing(ms): %f\n", ((double)(toc - tic)*(1000)) / CLOCKS_PER_SEC);

    exit(0);
}
