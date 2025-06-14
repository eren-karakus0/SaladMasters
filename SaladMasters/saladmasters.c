#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUM_CHEFS 6
#define MAX_LINE 10

typedef struct {
    char *filename;
} InputArgs;

typedef struct {
    int id;
    char has1;
    char has2;
} Chef;

// Küresel değişkenler
char material1, material2;
int current_line = 0;
char lines[MAX_LINE][3];
bool done = false;

sem_t chef_sems[NUM_CHEFS];
sem_t mutex;
sem_t ready_sem;

Chef chefs[NUM_CHEFS] = {
    {0, 'Y', 'T'},
    {1, 'Y', 'S'},
    {2, 'Y', 'L'},
    {3, 'T', 'S'},
    {4, 'T', 'L'},
    {5, 'S', 'L'}
};

int find_chef(char m1, char m2) {
    for (int i = 0; i < NUM_CHEFS; i++) {
        if (!((chefs[i].has1 == m1 || chefs[i].has2 == m1) ||
              (chefs[i].has1 == m2 || chefs[i].has2 == m2))) {
            return i;
        }
    }
    return -1;
}

void* chef_thread(void* arg) {
    Chef* chef = (Chef*)arg;
    char need1, need2;

    if ((chef->has1 == 'Y' && chef->has2 == 'T') || (chef->has1 == 'T' && chef->has2 == 'Y')) {
        need1 = 'L'; need2 = 'S';
    } else if ((chef->has1 == 'Y' && chef->has2 == 'S') || (chef->has1 == 'S' && chef->has2 == 'Y')) {
        need1 = 'T'; need2 = 'L';
    } else if ((chef->has1 == 'Y' && chef->has2 == 'L') || (chef->has1 == 'L' && chef->has2 == 'Y')) {
        need1 = 'S'; need2 = 'T';
    } else if ((chef->has1 == 'T' && chef->has2 == 'S') || (chef->has1 == 'S' && chef->has2 == 'T')) {
        need1 = 'Y'; need2 = 'L';
    } else if ((chef->has1 == 'T' && chef->has2 == 'L') || (chef->has1 == 'L' && chef->has2 == 'T')) {
        need1 = 'Y'; need2 = 'S';
    } else {
        need1 = 'Y'; need2 = 'T';
    }

    while (1) {
        printf("usta%d %c ve %c için bekliyor.\n", chef->id + 1, need1, need2);
        sem_wait(&chef_sems[chef->id]);
        if (done) break;

        printf("usta%d %c aldı.\n", chef->id + 1, need1);
        printf("usta%d %c aldı.\n", chef->id + 1, need2);
        printf("usta%d salatayı hazırlıyor.\n", chef->id + 1);

        sleep(rand() % 5 + 1);

        printf("usta%d salatayı toptancıya teslim etti.\n", chef->id + 1);
        sem_post(&ready_sem);
    }
    return NULL;
}

void* wholesaler_thread(void* arg) {
    InputArgs* args = (InputArgs*)arg;

    FILE* fp = fopen(args->filename, "r");
    if (!fp) {
        fprintf(stderr, "Dosya açılamadı.\n");
        exit(1);
    }

    while (fgets(lines[current_line], sizeof(lines[current_line]), fp)) {
        lines[current_line][strcspn(lines[current_line], "\n")] = 0;
        current_line++;
        if (current_line >= MAX_LINE) break;
    }
    fclose(fp);

    for (int i = 0; i < current_line; i++) {
        material1 = lines[i][0];
        material2 = lines[i][1];

        printf("toptancı %c ve %c teslim ediyor.\n", material1, material2);

        int chef_id = find_chef(material1, material2);
        if (chef_id >= 0) {
            sem_post(&chef_sems[chef_id]);
            printf("toptancı salatayı bekliyor.\n");
            sem_wait(&ready_sem);
            printf("toptancı salatayı aldı ve satmaya gitti.\n");
        } else {
            fprintf(stderr, "Uygun usta bulunamadı.\n");
        }
    }

    // Tüm ustalara işi bitti sinyali gönder
    done = true;
    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_post(&chef_sems[i]);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3 || strcmp(argv[1], "-i") != 0) {
        fprintf(stderr, "Kullanım: %s -i input.txt\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    pthread_t chefs_thread[NUM_CHEFS];
    pthread_t wholesaler;

    InputArgs args;
    args.filename = argv[2];

    sem_init(&ready_sem, 0, 0);

    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_init(&chef_sems[i], 0, 0);
        pthread_create(&chefs_thread[i], NULL, chef_thread, (void*)&chefs[i]);
    }

    pthread_create(&wholesaler, NULL, wholesaler_thread, (void*)&args);

    pthread_join(wholesaler, NULL);

    for (int i = 0; i < NUM_CHEFS; i++) {
        pthread_join(chefs_thread[i], NULL);
        sem_destroy(&chef_sems[i]);
    }

    sem_destroy(&ready_sem);
    return 0;
}
