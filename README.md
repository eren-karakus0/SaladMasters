# Distributed Salad Making System

This project simulates a synchronized salad-making system using POSIX threads and semaphores to solve a classic multithreading synchronization problem. 

## Overview

The system involves one **Wholesaler** and six **Salad Chefs**. The goal is to prepare salads, but there's a catch: to make a salad, four ingredients are required:
* Oil (Y)
* Salt (T)
* Lemon (L)
* Greens (S)

Each of the six chefs has an unlimited supply of only two out of the four ingredients. The Wholesaler has an unlimited supply of all ingredients but can only provide two at a time.

The synchronization challenge is to ensure that when the Wholesaler provides two ingredients, the correct Chef (the one missing precisely those two ingredients) receives them, prepares the salad, and gives it back to the Wholesaler.

## How It Works

1.  **The Chefs**: There are 6 chefs, each possessing two unique ingredients. They perpetually wait for the two ingredients they are missing. [cite: 9] The ingredient distribution is as follows:
    * **Chef 1**: Has Oil (Y) and Salt (T). Waits for Lemon (L) and Greens (S).
    * **Chef 2**: Has Oil (Y) and Greens (S). Waits for Salt (T) and Lemon (L).
    * **Chef 3**: Has Oil (Y) and Lemon (L). Waits for Greens (S) and Salt (T).
    * **Chef 4**: Has Salt (T) and Greens (S). Waits for Oil (Y) and Lemon (L).
    * **Chef 5**: Has Salt (T) and Lemon (L). Waits for Oil (Y) and Greens (S).
    * **Chef 6**: Has Greens (S) and Lemon (L). Waits for Oil (Y) and Salt (T).

2.  **The Wholesaler**: The Wholesaler reads a pair of ingredients from an input file. It then delivers these two ingredients, signaling the corresponding chef who needs them. After the chef prepares the salad, the wholesaler takes it and "goes to sell it" before reading the next pair of ingredients.

3.  **Synchronization**: The entire process is managed using semaphores to prevent race conditions and ensure orderly production. A dedicated semaphore is used for each chef, and another is used to signal the wholesaler that a salad is ready.

## Technical Details

* **Threading**: The program uses a total of 7 threads: 6 for the chefs and 1 for the wholesaler.
* **Synchronization**: All synchronization is handled by POSIX semaphores. 
* **Salad Preparation**: The time it takes to prepare a salad is a random duration between 1 and 5 seconds.
* **Termination**: All threads are gracefully terminated at the end of the program.

## Requirements

* Linux Operating System
* GCC Compiler
* Support for POSIX pthreads and semaphores 

## Getting Started

### Compilation

Navigate to the project directory in your terminal and compile the program using the following command:
```shell
gcc -pthread -o program saladmasters.c
```

### Execution

Run the program with the following command, specifying the path to the ingredient list:
```shell
./program -i input.txt
```
Here, `input.txt` is the file containing the ingredient delivery plans for the wholesaler.

## Input File Format

The input file (`input.txt`) must contain at least 10 lines. Each line must consist of exactly two characters, representing the ingredients the wholesaler will provide. 

**Valid Characters**:
* `Y`: Oil
* `T`: Salt
* `L`: Lemon
* `S`: Greens

**Example `input.txt`**:
```
TL
YS
LS
ST
LY
```
This corresponds to the wholesaler first delivering "Salt and Lemon", then "Oil and Greens", and so on.

## Example Output

The program's output will show the status of the chefs and the wholesaler.

**Sample run**:
```
chef2 is waiting for salt and lemon.
chef3 is waiting for greens and salt.
wholesaler delivers salt and lemon.
chef2 took the salt.
chef2 took the lemon.
chef2 is preparing the salad.
chef2 delivered the salad to the wholesaler.
wholesaler took the salad and went to sell it.
```