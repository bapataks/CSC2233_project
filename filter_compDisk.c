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
#include <stdbool.h>

#define BUFSIZE 20 // test
//#define BUFSIZE 4096 //4K

bool filterString(char *str)
{
    return strlen(str) < 20;
}

void compute(char *buf, char **tmp, int *size, int read_bytes, char **out, int *outLength)
{
    //char *tmp;
    int i,prev;

    for (i=0, prev=0; i<read_bytes; i++)
    {
        //printf("%c", buf[i]);
        if (buf[i] == '\n')
        {
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

            if (filterString(*tmp))
            {
                //printf("filtered string:%s\nsize:%d\n", *tmp, *size);
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

            //printf("free tmp pointer\n");
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
    char *app_reader = "/tmp/app_reader";
    char *app_writer = "/tmp/app_writer";

    int read_fd, write_fd, size=0, outLength=0;
    char *buf, *tmp;
    char *out;
    buf = (char*) malloc(BUFSIZE*sizeof(char));

    if (!access(app_writer, F_OK) == 0)
    {
        mkfifo(app_writer, 0666);
    }

    if (!access(app_reader, F_OK) == 0)
    {
        mkfifo(app_reader, 0666);
    }

    printf("Pipes ready\n");

    read_fd = open(app_writer, O_RDONLY);
    printf("File open to be read: %d\n", read_fd);
    if (read_fd < 0)
    {
        printf("%s:Could not open %s for reading %s\n", argv[0], app_writer, strerror(errno));
        exit(1);
    }

    int read_bytes = 0;
    clock_t tic = clock();
    read_bytes = read(read_fd, buf, BUFSIZE);
    while (read_bytes > 0)
    {
        buf[read_bytes] = '\0';
        compute(buf, &tmp, &size, read_bytes, &out, &outLength);
        //printf("out:%s\noutLength:%d\naddr:%p\n", out, outLength, (void*)out);
        read_bytes = read(read_fd, buf, BUFSIZE);
    }

    clock_t toc = clock();
    close(read_fd);

    printf("Final filtered output:\n%s\n", out);

    write_fd = open(app_reader, O_WRONLY);
    printf("file open to be written: %d\n", write_fd);
    if (write_fd < 0)
    {
        printf("%s:Could not open %s for writing %s\n", argv[0], app_reader, strerror(errno));
        exit(1);
    }

    int result = write(write_fd, out, outLength*sizeof(char));

    if (result < 0)
    {
        printf("%s:Could not write %s\n", argv[0], strerror(errno));
        exit(1);
    }

    close(write_fd);

    printf("Elapsed time in processing(ms): %f\n", ((double)(toc - tic)*(1000)) / CLOCKS_PER_SEC);
    exit(0);
}
