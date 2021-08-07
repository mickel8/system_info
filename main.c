#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>

const char *argp_program_version = "system_info 0.1.0";
const char *argp_program_bug_address = "michalsledz34@gmail.com";
static char doc[] = "system_info -- measure your system CPU load";
static struct argp_option options[] = {
        {"file",     'f', "FILE",     0, "Save results to a file"},
        {"interval", 'i', "INTERVAL", 0, "Frequency of measurements in seconds. Default to 1 second."},
        {"time",     't', "TIME",     0, "Time of measurement in seconds. Default to 60 seconds."},
        {0}
};

struct arguments {
    char *file;
    unsigned int interval;
    unsigned int time;
};

typedef int cpu_info_t[10];

void save_to_csv(char *file_name, float *cpu_loads, unsigned int size);

void print_args(struct arguments arguments);

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    int interval, time;
    struct arguments *arguments = state->input;
    switch (key) {
        case 'f':
            arguments->file = arg;
            break;
        case 'i':
            interval = atoi(arg);
            if (interval < 1) {
                printf("Interval cannot be lower than 1\n");
                argp_usage(state);
            }
            arguments->interval = (unsigned int) interval;
            break;
        case 't':
            time = atoi(arg);
            if (time < 1) {
                printf("Time cannot be lower than 1\n");
                argp_usage(state);
            }
            arguments->time = (unsigned int) time;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

int main(int argc, char **argv) {
    struct arguments arguments;
    arguments.file = "cpu_load.csv";
    arguments.interval = 1;
    arguments.time = 10;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    print_args(arguments);

    unsigned int iterations = arguments.time / arguments.interval;
    printf("Iterations: %d\n", iterations);
    char cpu[10] = {0};
    cpu_info_t last_cpu_info = {0};
    cpu_info_t current_cpu_info = {0};
    FILE *stat;
    float *cpu_loads = calloc(iterations, sizeof(float));;

    stat = fopen("/proc/stat", "r");
    fscanf(stat, "%s %d %d %d %d %d %d %d %d %d %d", cpu, &last_cpu_info[0],
           &last_cpu_info[1], &last_cpu_info[2], &last_cpu_info[3],
           &last_cpu_info[4], &last_cpu_info[5], &last_cpu_info[6],
           &last_cpu_info[7], &last_cpu_info[8], &last_cpu_info[9]);
    fclose(stat);
    for (int i = 0; i < iterations; i++) {
        sleep(arguments.interval);
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

    save_to_csv(arguments.file, cpu_loads, iterations);
    return 0;
}

void print_args(struct arguments arguments) {
    fprintf(stdout, "Output: %s\n"
                    "Interval: %d\n"
                    "Time: %d\n", arguments.file, arguments.interval, arguments.time);
}

void save_to_csv(char *file_name, float *cpu_loads, unsigned int size) {
    FILE *file = fopen(file_name, "w");
    for (int i = 0; i < size; i++) {
        // print to two decimal places
        fprintf(file, "%.2f\n", cpu_loads[i]);
    }
    fclose(file);
}
