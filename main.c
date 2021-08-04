#include <stdio.h>
#include <unistd.h>

typedef int cpu_info_t[10];
float get_cpu_load(cpu_info_t cpu_info);

int main(int argc, char **argv) {

  FILE *stat = fopen("/proc/stat", "r");
  char cpu[10] = {0};
  char cpu2[10] = {0};
  cpu_info_t cpu_info = {0};
  cpu_info_t cpu_info2 = {0};
  int ret = fscanf(stat, "%s %d %d %d %d %d %d %d %d %d %d", cpu, &cpu_info[0],
                   &cpu_info[1], &cpu_info[2], &cpu_info[3], &cpu_info[4],
                   &cpu_info[5], &cpu_info[6], &cpu_info[7], &cpu_info[8],
                   &cpu_info[9]);

  printf("%s %d %d %d %d %d %d %d %d %d %d\n", cpu, cpu_info[0], cpu_info[1],
         cpu_info[2], cpu_info[3], cpu_info[4], cpu_info[5], cpu_info[6],
         cpu_info[7], cpu_info[8], cpu_info[9]);

  fclose(stat);
  sleep(1);
  stat = fopen("/proc/stat", "r");
  ret = fscanf(stat, "%s %d %d %d %d %d %d %d %d %d %d", cpu2, &cpu_info2[0],
               &cpu_info2[1], &cpu_info2[2], &cpu_info2[3], &cpu_info2[4],
               &cpu_info2[5], &cpu_info2[6], &cpu_info2[7], &cpu_info2[8],
               &cpu_info2[9]);

  printf("%s %d %d %d %d %d %d %d %d %d %d\n", cpu2, cpu_info2[0], cpu_info2[1],
         cpu_info2[2], cpu_info2[3], cpu_info2[4], cpu_info2[5], cpu_info2[6],
         cpu_info2[7], cpu_info2[8], cpu_info2[9]);

  long busy_time1 = cpu_info[0] + cpu_info[1] + cpu_info[2] + cpu_info[5] +
                  cpu_info[6] + cpu_info[7];
  long total_time1 = busy_time1 + cpu_info[3] + cpu_info[4];

  long busy_time2 = cpu_info2[0] + cpu_info2[1] + cpu_info2[2] + cpu_info2[5] +
                  cpu_info2[6] + cpu_info2[7];
  long total_time2 = busy_time2 + cpu_info2[3] + cpu_info2[4];

  float totald = total_time2 - total_time1;
  float busyd = busy_time2 - busy_time1;
  printf("totald: %f, busyd: %f", totald, busyd);
  float cpu_load = busyd / totald * 100.0f;
  printf("CPU load: %f%%\n", cpu_load);
  return 0;
}
