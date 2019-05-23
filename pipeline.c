#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

static sem_t sem;

static int queue[4096] = {0,};
static int queue_length = 0;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

static int queue_idx = 0;
static pthread_mutex_t queue_visitor_mutex = PTHREAD_MUTEX_INITIALIZER;

static void enqueue(int v)
{
  pthread_mutex_lock(&queue_mutex);
  queue[queue_length++] = v;
  pthread_mutex_unlock(&queue_mutex);
}

void* sum_thrd_main(void* arg)
{
  sem_wait(&sem);

  pthread_mutex_lock(&queue_visitor_mutex);
  for (; queue_idx < queue_length; queue_idx++) {
    printf("%d\n", queue[queue_idx]);
  }
  pthread_mutex_unlock(&queue_visitor_mutex);

  pthread_exit(0);
}

int main()
{
  char buf[4096];

  FILE* fp = fopen("./data/numbers.txt", "r");
  if (!fp) {
    fprintf(stderr, "Failed to open ./data/numbers.txt\n");
    exit(1);
  }

  pthread_t tid[4];
  for (int i = 0; i < 4; ++i) {
    pthread_create(&tid[i], NULL, sum_thrd_main, NULL);
  }

  int a, b, c, d;

  while (fscanf(fp, "%d %d %d %d", &a, &b, &c, &d) != EOF) {
    enqueue(a);
    enqueue(b);
    enqueue(c);
    enqueue(d);

    sem_post(&sem);
  }

  for (int i = 0; i < 4; ++i) {
    pthread_join(tid[i], NULL);
  }

  fclose(fp);

  return 0;
}
