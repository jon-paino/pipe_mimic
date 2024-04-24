## UID: 105897318

## Pipe Up

This program mimics the behavior of the standard pipe operator within the shell by manually piping together
programs via file descriptor manipulation with parent and child processes.

## Building

```
make
```
This will build the executable pipe

## Running

```
./pipe ls 
```
This will mimic the behavior of running the ls command with the shell which obviously outputs the contents of your current directory

```
./pipe ls wc 
```
This will mimic the behavior of running ls | wc which will output the number of lines, words, and characters in from the output of ls

## Cleaning up
``` 
make clean 
``` 
will remove the binaries made during the build process
