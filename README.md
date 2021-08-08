# system_info

Simple program for measuring CPU load for a period of time.


## Requirements

Linux kernel version >= 2.6.33

## Compile and run

Compile with:

```bash
make compile
```

Run with:

```bash
# measure cpu load every 1 second for 60 seconds and save results to a file with unix timestamp name
./system_info

# the same as above but with interval of 2 seconds
./system_info --interval 2

# measure cpu load every 2 seconds for 20 seconds and save it to a file `cpu_load.csv`
./system_info --file cpu_load.csv --time 20 --interval 2
```

Other combinations of options are possible. 
For more information refer to: 

```bash
./system_info --help
```

