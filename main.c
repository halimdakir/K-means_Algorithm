#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ITERATIONS 1000000 // Maximum number of iterations
#define PREDEFINED_PATH "kmeans-data.txt" // Default input data file 
int k;

// Defining a structure to store the Cartesian coordinates of a point
typedef struct {
    double x;
    double y;
} Point;

// Defining a structure to represent a cluster of points
typedef struct {
    Point centroid;
    int num_points;
    Point* points;
} Cluster;

// Function prototypes
Point** initializeCentroids(int k);
Cluster* initializeClusters(int k);
int get_number_of_clusters();
int read_file_to_get_points(const char *filename, Point** points); 
void initialize_random_clusters(Cluster clusters[], int k, Point points[], int num_points);
double get_eucladian_distance(Point a, Point b);
void assign_points_to_clusters(Point points[], int num_points, Cluster clusters[], int k);
void update_centroids(Cluster clusters[], int k);
bool centroids_changed(Cluster clusters[], Cluster old_clusters[], int k);
void write_clusters(const char *filename, Cluster clusters[], int k, Point points[], int num_points);
void get_user_centroids(Cluster clusters[], int k, Point** centroids, Point points[], int num_points);
void initialize_custom_clusters(Cluster clusters[], Point** centroids, int k, Point points[], int num_points);
void freeCentroidMemory(int k, Point** centroids);
bool is_valid_txt_file(const char *path);
char* get_file_path();

int main() {
    // Getting the path based on user's choice: default/user specified
    // Allocate memory for a string to hold the file path (256 bytes)
    char* file_path = malloc(256);
    // Copy the file path
    strcpy(file_path, get_file_path(file_path));

    // Call the function to get a valid number of clusters
    k = get_number_of_clusters();
    Cluster* clusters = initializeClusters(k);
    Point** centroids = initializeCentroids(k);

    // Change to pointer for dynamic allocation
    Point* points = NULL;

    // Invoke the function to read the input file to get data points
    int num_points = read_file_to_get_points(file_path, &points);
    // If no valid data found in the file
    if (num_points <= 0){
      printf("No data found to cluster!");
      exit(1);
    }
    
    // Taking number of cluser again if specified cluster number is less than number of valid points in the file
    if(num_points < k){
        while(k <= 0 || num_points < k){
          printf("Please give a number between 1 and %d: ", num_points);
          scanf("%d", &k); 
        }
    }

    while (1) {
        char response[4];
        printf("Do you want to enter the initial centroids? (yes/no): ");
        scanf("%3s", response);
        
        // Only proceed if user agrees to provide centroids
        if (strcmp(response, "yes") == 0) {
            get_user_centroids(clusters, k, centroids, points, num_points);
            break;
        } else if(strcmp(response, "no") == 0){
            initialize_random_clusters(clusters, k, points, num_points);
            break;
        } else{
            printf("Invalid input. Please enter 'yes' or 'no'.\n");
        }
    }
    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // Store the old cluster centroids
        Cluster old_clusters[k];
        for (int j = 0; j < k; j++) {
            old_clusters[j] = clusters[j];
            clusters[j].num_points = 0; // Reset the number of points in each cluster
        }
        
        assign_points_to_clusters(points, num_points, clusters, k);
        update_centroids(clusters, k);

        // Check for convergence & Centroids have not changed, exit loop
        if (!centroids_changed(clusters, old_clusters, k)) {
            break;
        }
    }

    // Write the clusting output in the output file
    write_clusters("kmeans-output.txt", clusters, k, points, num_points);

    //Free allocated memories
    free(points);
    free(file_path);
    freeCentroidMemory(k, centroids);

    for (int i = 0; i < k; i++) {
        free(clusters[i].points);
    }
    free(clusters);
    
    return 0;
}

Cluster* initializeClusters(int k) {
    Cluster* clusters = malloc(k * sizeof(Cluster));
    if (clusters == NULL) {
        fprintf(stderr, "Memory allocation failed for clusters\n");
        exit(1);
    }
    return clusters;
}

Point** initializeCentroids(int k) {
    Point** centroids = malloc(k * sizeof(Point*));
    if (centroids == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < k; i++) {
        centroids[i] = malloc(sizeof(Point));
        if (centroids[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            freeCentroidMemory(k, centroids);
            exit(1);
        }
    }
    return centroids;
}

// Function to prompt the user for the number of clusters and validate input
int get_number_of_clusters() {
    int k;

    printf("Enter number of clusters: ");

    // Loop until a valid integer is entered
    while (1) {

        if (scanf("%d", &k) != 1) {

            printf("Invalid input. Please enter a valid integer: ");
            while (getchar() != '\n');

        } else if (k <= 0) {

            printf("Please give a number greater than 0: ");

        } else {
            break;
        }
    }

    return k;
}

// Checking if the provided custom data file is valid or not
bool is_valid_txt_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        int len = strlen(path);
        return len > 4 && strcmp(path + len - 4, ".txt") == 0;
    }
    return false;
}

// Take the custom data file path from the user 
char* get_file_path(char* file_path) {
    char user_input[4];
    // Prompt until a valid response is given
    while (1) {
        printf("Do you want to specify a file path? (yes/no): ");
        scanf("%3s", user_input);

        // Check if input is "yes" or "no"
        if (strcmp(user_input, "yes") == 0) {
            printf("Please enter the file path: ");
            scanf("%255s", file_path);

            // Validate the file path
            if (is_valid_txt_file(file_path)) {
                return file_path;
            } else {
                printf("Invalid file path or the file is not in .txt format. Please enter a valid path.\n");
            }
        } else if (strcmp(user_input, "no") == 0) {
            printf("Using the default file as input for data: '%s'\n", PREDEFINED_PATH);
            break;
        } else {
            printf("Invalid input. Please enter 'yes' or 'no'.\n");
        }
    }

    // Copy predefined path to path array if user input is invalid
    strcpy(file_path, PREDEFINED_PATH);
    return file_path;
}

//Function to take centroid selection from the user
void get_user_centroids(Cluster clusters[], int k, Point** centroids, Point points[], int num_points) {
    printf("Enter %d pairs of x and y coordinates for the centroids (e.g., '1.0 2.0'):\n", k);
        
    for (int i = 0; i < k; i++) {
        printf("Centroid %d: ", i + 1);
        // Validate that exactly two floating-point numbers are provided
        if (scanf("%lf %lf", &centroids[i]->x, &centroids[i]->y) != 2) {
            printf("Invalid input. Please enter two floating-point numbers separated by a space.\n");
            i--;
            while (getchar() != '\n');
            continue;
        }
    }

    initialize_custom_clusters(clusters, centroids, k, points, num_points);
}

// Function to free memory allocated for centroids 
void freeCentroidMemory(int k, Point** centroids){
    for (int i = 0; i < k; i++){
        free(centroids[i]);
    }
    free(centroids);
}