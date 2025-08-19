#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SIZE (1 << 16)
#define MAX_CHILDREN 16

int array[SIZE];

int main(int argc, char *argv[]) {
  int n = 4;  // number of child processes
  int pids[MAX_CHILDREN];
  int pipes[MAX_CHILDREN][2];  

  for (int i = 0; i < SIZE; i++)
    array[i] = i;

  // Create pipes before forking
  for (int i = 0; i < n; i++) {
    if (pipe(pipes[i]) < 0) {
      printf("pipe creation failed\n");
      exit(1, "pipe error");
    }
  }

  // Fork children
  int res = forkn(n, pids);
  if (res < 0) {
    printf("forkn failed\n");
    exit(1, "forkn failed");
  }

  if (res > 0) {
    // Child process
    int idx = res - 1;
    int from = idx * (SIZE / n);
    int to = from + (SIZE / n);
    int sum = 0;
    for (int i = from; i < to; i++)
      sum += array[i];

    printf("Child %d (pid %d): Calculating sum from %d to %d\n", idx + 1, getpid(), from, to);

    for (int i = 0; i < n; i++) {
      close(pipes[i][0]);
      if (i != idx) close(pipes[i][1]);  
    }

    // Write full int sum to pipe
    write(pipes[idx][1], &sum, sizeof(int));
    close(pipes[idx][1]);

    printf("Child %d (pid %d): sum = %d\n", idx + 1, getpid(), sum);
    exit(0, ""); 
  } else {
    // Parent process
    printf("Parent: created children with PIDs: ");
    for (int i = 0; i < n; i++)
      printf("%d ", pids[i]);
    printf("\n");

    // Close write ends in parent
    for (int i = 0; i < n; i++)
      close(pipes[i][1]);

    // Wait for all children
    int statuses[MAX_CHILDREN];
    int completed = 4;
    waitall(&completed, statuses);

    int total = 0;
    for (int i = 0; i < n; i++) {
      int sum;
      int bytesRead = read(pipes[i][0], &sum, sizeof(int));
      if (bytesRead != sizeof(int)) {
        printf("Parent: failed to read from pipe %d (bytes read: %d)\n", i, bytesRead);
        exit(1, "read error");
      }
      total += sum;
      close(pipes[i][0]);
      printf("Parent: partial sum from child %d = %d\n", i, sum);
    }

    printf("Parent: total sum of all = %d\n", total);  // Expect 2147450880
    exit(0, "calculation completed"); 
  }
}
