# K-means Clustering Using C Programming Language


## Course Details
- **Course:** Programming in the UNIX Environment


## Objective
This lab aims to deepen understanding and practical skills in C programming within a UNIX environment, focusing on:
- Dynamic memory allocation
- Pointer manipulation
- Array handling
- File I/O
- Implementing the K-means clustering algorithm for 2D data points

## Requirements
- **Language:** C
- **Compiler:** GCC
- **Environment:** UNIX, Ubuntu 24.04 or Fedora 40

## Description
Implement the K-means clustering algorithm to cluster 2-dimensional data points based on user-specified cluster counts. This includes reading data, processing it to form clusters, and outputting the results.

## Files
- `main.c`: Main program that handles input, clustering, and output operations.
- `kmeans.c`: Contains the K-means clustering algorithm implementation.
- `Makefile`: Used to compile and link the program.
- `kmeans-data.txt`: Input file containing the data to be clustered.
- `kmeans-output.txt`: Output file showing the clustering results.

## Usage
1. Clone this repository or download the project to your local machine.
2. Navigate to the project directory in your terminal.
3. Run `make` to compile the program.
4. Execute the program using `./kmeans`.
5. Check `kmeans-output.txt` for the clustering results.

## Build
To build the project, run:
```bash
make clean

make

./kmeans

