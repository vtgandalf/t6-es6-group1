#include<stdio.h> 
#include<fcntl.h>

#define MAX_BUF_LEN     (255)
#define INDEX_DEVFILE   (1)
#define INDEX_DATA      (2)

int main(int argc, char** argv)
{ 
    if (argc < INDEX_DATA)
    {
        return -1;
    }

    char* dev_file = argv[INDEX_DEVFILE];

    int write_data;

    if (sscanf(argv[INDEX_DATA], "%d", &write_data) < 1)
    {
        return -1;
    }

    int fd = open(dev_file, O_RDWR);

    if (fd < 0)
    {
        return -1;
    }

    int retval = 0;

    char buffer[MAX_BUF_LEN];
    
    if (sprintf(buffer,"%d", write_data) > 0)
    {
        retval = write (fd, buffer, sizeof(buffer));
    }

    if (retval == sizeof(buffer))
    {
        int data;
        
        int bytes_read = read(fd, &data, sizeof(int));
    
        if (bytes_read > 0)
        {
            printf ("data = %d, read %d bytes\n", data, bytes_read);
        }
    }

    close (fd);

    return 0;
} 