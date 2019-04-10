#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef VERBOSE_ENABLED
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) \
  do {           \
  } while (false);
#endif
struct solution_state {
  int *vals;
} * solution_state_val;

int safe_state_counter = 0;
struct system_state {
  int resource_count;
  int process_count;
  int *avail_resource;
  int **allocation_table;
  int **max_table;

} * global_system_state;
void free_dynamic_resource() {
  for (int a = 0; a < global_system_state->process_count; a++) {
    free(global_system_state->allocation_table[a]);
    free(global_system_state->max_table[a]);
  }
  free(global_system_state->allocation_table);
  free(global_system_state->max_table);
  free(global_system_state->avail_resource);
  free(solution_state_val->vals);
  free(global_system_state);
  free(solution_state_val);
}
void input() {
  LOG("\nStarted input function...");

  global_system_state =
      (struct system_state *)malloc(sizeof(struct system_state));

  LOG("\nAllocated global_system_state variable...");
  printf("Enter Total Process Count : ");
  scanf("%d", &(global_system_state->process_count));
  printf("\nEnter Total Resource Count : ");
  scanf("%d", &(global_system_state->resource_count));
  LOG("\nRead Process and resource counts...");
  printf(
      "\nEnter Allocated Resource Count as Table (Process in rows, and "
      "Resource in columns) : \n");
  LOG("\nAllocating memory for allocation table...");

  global_system_state->allocation_table =
      (int **)malloc(global_system_state->process_count * sizeof(int *));

  for (int a = 0; a < global_system_state->process_count; a++)
    global_system_state->allocation_table[a] =
        (int *)malloc(global_system_state->resource_count * sizeof(int));

  LOG("\nAllocated allocation table memory now reading...");
  
  for (int a = 0; a < global_system_state->process_count; a++)
    for (int b = 0; b < global_system_state->resource_count; b++)
      scanf("%d", &(global_system_state->allocation_table[a][b]));
  LOG("\nMax Table Allocating memory...");
  

  global_system_state->max_table =
      (int **)malloc(global_system_state->process_count * sizeof(int *));

  for (int a = 0; a < global_system_state->process_count; a++)
    global_system_state->max_table[a] =
        (int *)malloc(global_system_state->resource_count * sizeof(int));

  LOG("\nAllocated max table memory now reading...");
  printf(
      "\nEnter Maximum Resource Count Limit as Table (Process in rows, and "
      "Resource limit in columns) : \n");

  for (int a = 0; a < global_system_state->process_count; a++)
    for (int b = 0; b < global_system_state->resource_count; b++)
      scanf("%d", &(global_system_state->max_table[a][b]));

  LOG("\nAllocating memory to avail_resources...");

  global_system_state->avail_resource =
      (int *)malloc(sizeof(int) * global_system_state->resource_count);

  LOG("\nAllocating memory to solution_state...");

  solution_state_val =
      (struct solution_state *)malloc(sizeof(struct solution_state));

  LOG("\nAllocating memory to solution state value...");

  solution_state_val->vals =
      (int *)malloc(sizeof(int) * global_system_state->process_count);

  LOG("\nReading values to available_resources...");
  printf(
      "\nEnter available resource count in the same order as above for each "
      "resource : \n");
  for (int a = 0; a < global_system_state->resource_count; a++)
    scanf("%d", &(global_system_state->avail_resource[a]));
};

bool vec_math_is_allocatable(int *a, int *b, int *c, int len) {
  for (int iter = 0; iter < len; iter++)
    if (a[iter] + b[iter] < c[iter]) return false;
  return true;
}

void vec_math_allocate_and_free(int *a, int *b, int *c, int len) {
  assert(vec_math_is_allocatable(a, b, c, len));
  for (int iter = 0; iter < len; iter++) {
    a[iter] += b[iter];
    b[iter] = 0;
  }
}

void solve() {
  LOG("\nStarted solving the system...");
  int non_executed = global_system_state->process_count;
  bool dead_lock = false;
  bool has_completed[global_system_state->process_count];

  for (int a = 0; a < global_system_state->process_count; a++)
    has_completed[a] = false;

  while (non_executed) {
    dead_lock = true;
    for (int a = 0; a < global_system_state->process_count; a++) {
      if (has_completed[a]) continue;
      LOG("\nChecking if P%d can be allocated for execution...", a);
      bool res =
          vec_math_is_allocatable(global_system_state->avail_resource,
                                  global_system_state->allocation_table[a],
                                  global_system_state->max_table[a],
                                  global_system_state->resource_count);
      if (res) {
        has_completed[a] = true;
        non_executed--;
        dead_lock = false;
        solution_state_val->vals[safe_state_counter++] = a;
        LOG("\nAllocating and releasing resources for P%d", a);
        vec_math_allocate_and_free(global_system_state->avail_resource,
                                   global_system_state->allocation_table[a],
                                   global_system_state->max_table[a],
                                   global_system_state->resource_count);
      } else {
        LOG("\nCannot satisfy needs for P%d. Skipping...", a);
      }
    }
    if (dead_lock) {
      LOG("\nEncountered a deadlock...");
      free_dynamic_resource();
      LOG("\nFreed all dynamic resources...");
      solution_state_val = NULL;
      break;
    }
  }
};

void result() {
  if (solution_state_val == NULL) {
    printf(
        "\nDEAD LOCK : System is not in Stable State. Some process must free "
        "some resources to resolve this issue");
  } else {
    printf(
        "\nNo Dead Lock Found. System is in valid state if following order of "
        "process execution is followed : \n\t");
    for (int a = 0; a < global_system_state->process_count; a++) {
      if (a == global_system_state->process_count - 1)
        printf("P%d", solution_state_val->vals[a]);
      else
        printf("P%d->", solution_state_val->vals[a]);
    }
    free_dynamic_resource();
  }
  printf("\n");
}
int main() {
  input();
  solve();
  result();
  return 0;
}