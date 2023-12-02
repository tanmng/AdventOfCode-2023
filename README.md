# Advent of code 2023


My attempt at solving Advent of code 2023.

The code are written in CPP

## Build and run the code

I use Bazel to run these code, so usually you can run the code with command as below

```sh
bazel run :day02 -- < day02/input
```

The command should produce the required output

```
Part 1: 2617
Part 2: 59795
```

Sometimes I write the code to be as efficient as I could, which means you have to run it twice to solve 2 parts of the puzzle, in such case, the program will print out a hint to use `--part2` when invoking it. Example:

```
bazel run :day01 -- < day01/input
```

```
Part 1: 54573
Solution above is for part 1, if you need part 2 solution, add the flag --part2 to the command invocation
```

You can then solve part2 by using following command

```sh
bazel run :day01 -- --part2 < day01/input
```

Output

```
Part 2: 54591
```

## Some weirdness

The code is originally written in my personal monorepo.  The mono repo is too embarassing (way too much bad code), and also for previous years I just keep creating dedicated repo for it, so I create this repo and sync the code over. I try my best to keep the code runnable but please pardon any issues
