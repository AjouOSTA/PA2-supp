#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pthread.h>

static char buf[4096];
static int buf_len = 0;

static char buf_for_parallel[4096] = {0,};
const int k_num_thread = 4;
static int counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void* parallel_str_transform(void* arg)
{
  int id;

  pthread_mutex_lock(&counter_mutex);
  id = counter++;
  pthread_mutex_unlock(&counter_mutex);

  for (int i = id; i < buf_len; i += k_num_thread) {
    if (!isalnum(buf[i])) {
      buf_for_parallel[i] = ' ';
    } else {
      buf_for_parallel[i] = tolower(buf[i]);
    }
  }

  pthread_exit(0);
}

int main()
{
  FILE* fp = fopen("./data/strings.txt", "r");
  if (!fp) {
    fprintf(stderr, "Failed to open ./data/strings.txt\n");
    exit(1);
  }

  fgets(buf, 4096, fp);

  printf("%s\n", buf);

  buf_len = strlen(buf);

  // Serial
  char buf_for_serial[4096];

  for (int i = 0; i < buf_len; ++i) {
    if (!isalnum(buf[i])) {
      buf_for_serial[i] = ' ';
    } else {
      buf_for_serial[i] = tolower(buf[i]);
    }
  }

  printf("%s\n", buf_for_serial);

  // Parallel
  pthread_t pid[k_num_thread];

  for (int i = 0; i < k_num_thread; ++i) {
    pthread_create(&pid[i], NULL, parallel_str_transform, NULL);
  }

  for (int i = 0; i < k_num_thread; ++i) {
    pthread_join(pid[i], NULL);
  }

  printf("%s\n", buf_for_parallel);

  fclose(fp);

  return 0;
}
