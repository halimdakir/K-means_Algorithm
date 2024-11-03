#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ITERATIONS 1000000 // Maximum number of iterations
#define PREDEFINED_PATH "kmeans-data.txt"
int k;

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point centroid;
    int num_points;
    Point* points;  // Dynamic allocation of points
} Cluster;

// Function prototypes
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

int is_valid_txt_file(const char *path);
char* get_file_path(); // Add the prototype for get_file_path


int main() {
    char* file_path = malloc(256);
    strcpy(file_path, get_file_path(file_path));
    
    printf("Enter number of clusters: ");
    scanf("%d", &k);
    while(k <= 0){
        printf("Please give a number greater than 0: ");
        scanf("%d", &k); 
    }
    
    Point* points = NULL; // Change to pointer for dynamic allocation
    int num_points = read_file_to_get_points(file_path, &points);
    if (num_points <= 0){
      printf("No data found to cluster!");
      exit(1);
    }
    
    if(num_points < k){
        while(k <= 0 || num_points < k){
          printf("Please give a number between 1 and %d: ", num_points);
          scanf("%d", &k); 
        }
    }
    
    Cluster* clusters = malloc(k * sizeof(Cluster)); // Allocate memory for clusters
    if (clusters == NULL) {
        fprintf(stderr, "Memory allocation failed for clusters\n");
        exit(1);
    }
    
    //-------------------------------

    Point** centroids = malloc(k * sizeof(Point*));
    if (centroids == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
    }

    for (int i = 0; i < k; i++) {
        centroids[i] = malloc(sizeof(Point));
        if (centroids[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            freeCentroidMemory(k, centroids);
        }
    }

    char response[4];
    printf("Do you want to enter the initial centroids? (yes/no): ");
    scanf("%3s", response);
    
    // Only proceed if user agrees to provide centroids
    if (strcmp(response, "yes") == 0) {
        get_user_centroids(clusters, k, centroids, points, num_points);
    }else{
        initialize_random_clusters(clusters, k, points, num_points);
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

        // Check for convergence
        if (!centroids_changed(clusters, old_clusters, k)) {
            break; // Centroids have not changed, exit loop
        }
    }

    write_clusters("kmeans-output.txt", clusters, k, points, num_points);

    // Free allocated memory
    free(points); // Free the points array
    free(file_path);
    for (int i = 0; i < k; i++) {
        free(clusters[i].points); // Free each cluster's points
    }
    free(clusters); // Free the clusters array
    
    freeCentroidMemory(k, centroids);

    return 0;
}

int is_valid_txt_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        int len = strlen(path);
        return len > 4 && strcmp(path + len - 4, ".txt") == 0;
    }
    return 0;
}


char* get_file_path(char* file_path) {
    char user_input[4];
    printf("Do you want to specify a file path? (yes/no): ");
    scanf("%3s", user_input);

    if (strcmp(user_input, "yes") == 0) {
        printf("Please enter the file path: ");
        scanf("%255s", file_path);

        if (is_valid_txt_file(file_path)) {
            return file_path;
        } else {
            printf("Invalid file path or the file is not in .txt format. Using predefined path.\n");
        }
    }

    // Copy predefined path to path array if user input is invalid
    strcpy(file_path, PREDEFINED_PATH);
    return file_path;
}

void get_user_centroids(Cluster clusters[], int k, Point** centroids, Point points[], int num_points) {
    printf("Enter %d pairs of x and y coordinates for the centroids (e.g., '1.0 2.0'):\n", k);
        
    for (int i = 0; i < k; i++) {
        printf("Centroid %d: ", i + 1);
        // Validate that exactly two floating-point numbers are provided
        if (scanf("%lf %lf", &centroids[i]->x, &centroids[i]->y) != 2) {
            printf("Invalid input. Please enter two floating-point numbers separated by a space.\n");
            i--; // Repeat this iteration for incorrect input
            while (getchar() != '\n'); // Clear invalid input from buffer
            continue;
        }
    }

    initialize_custom_clusters(clusters, centroids, k, points, num_points);
}

void freeCentroidMemory(int k, Point** centroids){
    for (int i = 0; i < k; i++){
        free(centroids[i]);
    }
    free(centroids);
}