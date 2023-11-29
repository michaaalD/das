#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define BABBLE_NAME "kkucz"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80


struct babblespace
{
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

void wyswietl(struct babblespace *babble_wsk)
{
    pthread_mutex_lock(&babble_wsk->babble_mutex);
    printf("Komunikaty w pamięci:\n");
    int i = babble_wsk->babble_first;
    int licznik = 0;

    while (licznik < babble_wsk->babble_total)
    {
        printf("[%d]: %s\n", i + 1, babble_wsk->babbles[i]);
        i = (i + 1) % BABBLE_LIMIT;
        licznik++;
    }
    pthread_mutex_unlock(&babble_wsk->babble_mutex);
}

int main()
{
    int shmd;
    struct babblespace *babble_wsk;
    shmd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    if (shmd == -1)
    {
        perror("Błąd przy otwieraniu");
        exit(1);
    }

    ftruncate(shmd, sizeof(struct babblespace));

    babble_wsk = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    if (babble_wsk == MAP_FAILED)
    {
        perror("Błąd przy mapowaniu");
        exit(1);
    }

    wyswietl(babble_wsk);
    munmap(babble_wsk, sizeof(struct babblespace));

    if (close(shmd) == -1)
    {
        perror("Błąd przy zamykaniu");
        exit(1);
    }

    return 0;
}
