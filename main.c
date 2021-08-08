#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include <time.h>

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

struct cpu_info {
    char name[10];
    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
    long long irq;
    long long softirq;
    long long steal;
    long long guest;
    long long guest_nice;
};

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
    unsigned long timestamp = (unsigned long)time(NULL);
    int timestamp_len = snprintf(NULL, 0, "%lu", timestamp);
    char *file_name = (char *)malloc(timestamp_len + 1 + 4); // +1 for NULL byte and +4 for .csv extenstion 
    snprintf(file_name, timestamp_len + 1 + 4, "%lu.csv", timestamp);

    struct arguments arguments;
    arguments.file = file_name;
    arguments.interval = 1;
    arguments.time = 60;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    print_args(arguments);

    unsigned int iterations = arguments.time / arguments.interval;
    fprintf(stdout, "Iterations: %d\n", iterations);
    struct cpu_info last_cpu_info;
    struct cpu_info current_cpu_info;
    FILE *stat;
    float *cpu_loads = calloc(iterations, sizeof(float));;

    stat = fopen("/proc/stat", "r");
    fscanf(stat, "%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", last_cpu_info.name, &last_cpu_info.user,
           &last_cpu_info.nice, &last_cpu_info.system, &last_cpu_info.idle,
           &last_cpu_info.iowait, &last_cpu_info.irq, &last_cpu_info.softirq,
           &last_cpu_info.steal, &last_cpu_info.guest, &last_cpu_info.guest_nice);
    fclose(stat);
    for (int i = 0; i < iterations; i++) {
        sleep(arguments.interval);
        stat = fopen("/proc/stat", "r");
        fscanf(stat, "%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", current_cpu_info.name, &current_cpu_info.user,
               &current_cpu_info.nice, &current_cpu_info.system, &current_cpu_info.idle,
               &current_cpu_info.iowait, &current_cpu_info.irq, &current_cpu_info.softirq,
               &current_cpu_info.steal, &current_cpu_info.guest, &current_cpu_info.guest_nice);

        fclose(stat);

        long current_busy_time = current_cpu_info.user + current_cpu_info.nice +
                                 current_cpu_info.system + current_cpu_info.irq +
                                 current_cpu_info.softirq + current_cpu_info.steal;
        long current_total_time =
                current_busy_time + current_cpu_info.idle + current_cpu_info.iowait;

        long last_busy_time = last_cpu_info.user + last_cpu_info.nice +
                              last_cpu_info.system + last_cpu_info.irq +
                              last_cpu_info.softirq + last_cpu_info.steal;
        long last_total_time = last_busy_time + last_cpu_info.idle + last_cpu_info.iowait;

        float totald = current_total_time - last_total_time;
        float busyd = current_busy_time - last_busy_time;
        float cpu_load = busyd / totald * 100.0f;
        fprintf(stdout, "totald: %f, busyd: %f, load: %f%%\n", totald, busyd, cpu_load);
        cpu_loads[i] = cpu_load;
        last_cpu_info = current_cpu_info;
    }

    save_to_csv(arguments.file, cpu_loads, iterations);
    free(cpu_loads);
    free(file_name);
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
