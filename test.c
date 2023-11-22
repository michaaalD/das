

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define BABBLE_NAME "/Y01-42e"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
  pthread_mutex_t babble_mutex;
  pthread_cond_t babble_cond;
  int babble_first, babble_total;
  char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

int main() {
  int fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
  if (fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  struct babblespace *babbles = mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (babbles == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < babbles->babble_total; i++) {
    printf("%s\n", babbles->babbles[i]);
  }

  if (munmap(babbles, sizeof(struct babblespace)) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return 0;
}
