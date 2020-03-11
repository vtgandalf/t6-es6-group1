#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 

#define PWM1_ENABLE_DEV      "/dev/pwm1_enable"

int main(void)
{
  int fd;
  int bytes_read;
  int buffer;

  fd = open(PWM1_ENABLE_DEV, 0);

  if (fd < 0) {
    printf ("Can't open device file: %s\n", PWM1_ENABLE_DEV);
    return -1;
  }

  bytes_read = read(fd, &buffer, sizeof(int));

  if (bytes_read != 0)
  {
    printf("Read ok! buf=%d\n", buffer);
  }

  close(fd); 

  return 0;
}