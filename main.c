#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define INTERVAL 1
#define ITERATIONS 10

typedef int cpu_info_t[10];

float get_cpu_load(cpu_info_t cpu_info);

int main(int argc, char **argv) {

    int ret;
    char cpu[10] = {0};
    cpu_info_t last_cpu_info = {0};
    cpu_info_t current_cpu_info = {0};
    FILE *stat;
    float cpu_loads[10];

    stat = fopen("/proc/stat", "r");
    fscanf(stat, "%s %d %d %d %d %d %d %d %d %d %d", cpu, &last_cpu_info[0],
           &last_cpu_info[1], &last_cpu_info[2], &last_cpu_info[3],
           &last_cpu_info[4], &last_cpu_info[5], &last_cpu_info[6],
           &last_cpu_info[7], &last_cpu_info[8], &last_cpu_info[9]);
    fclose(stat);
    for (int i = 0; i < ITERATIONS; i++) {
        sleep(INTERVAL);
        stat = fopen("/proc/stat", "r");
        fscanf(stat, "%s %d %d %d %d %d %d %d %d %d %d", cpu, &current_cpu_info[0],
               &current_cpu_info[1], &current_cpu_info[2], &current_cpu_info[3],
               &current_cpu_info[4], &current_cpu_info[5], &current_cpu_info[6],
               &current_cpu_info[7], &current_cpu_info[8], &current_cpu_info[9]);

        fclose(stat);
        printf("%s %d %d %d %d %d %d %d %d %d %d\n", cpu, current_cpu_info[0],
               current_cpu_info[1], current_cpu_info[2], current_cpu_info[3],
               current_cpu_info[4], current_cpu_info[5], current_cpu_info[6],
               current_cpu_info[7], current_cpu_info[8], current_cpu_info[9]);

        long current_busy_time = current_cpu_info[0] + current_cpu_info[1] +
                                 current_cpu_info[2] + current_cpu_info[5] +
                                 current_cpu_info[6] + current_cpu_info[7];
        long current_total_time =
                current_busy_time + current_cpu_info[3] + current_cpu_info[4];

        long last_busy_time = last_cpu_info[0] + last_cpu_info[1] +
                              last_cpu_info[2] + last_cpu_info[5] +
                              last_cpu_info[6] + last_cpu_info[7];
        long last_total_time = last_busy_time + last_cpu_info[3] + last_cpu_info[4];

        float totald = current_total_time - last_total_time;
        float busyd = current_busy_time - last_busy_time;
        printf("totald: %f, busyd: %f", totald, busyd);
        float cpu_load = busyd / totald * 100.0f;
        printf("CPU load: %f%%\n", cpu_load);
        cpu_loads[i] = cpu_load;
        memcpy(last_cpu_info, current_cpu_info, 10 * sizeof(float));
    }
    return 0;
}
