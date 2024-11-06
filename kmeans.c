#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point centroid;
    int num_points;
    Point* points;  // Dynamic allocation of points
} Cluster;

extern int k;  // Declaration of the global variable k
allocate_memory_cluster(Cluster clusters[], int num_points, int i)

// Function to read points from a file
int read_file_to_get_points(const char *filename, Point** points) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    int count = 0;
    *points = malloc(k * sizeof(Point));  // Allocate initial memory for points
    
    char line[256];  // Buffer to hold each line of the file
    while (fgets(line, sizeof(line), file) != NULL) {
        double x, y;
        // Attempt to parse two doubles from the line
        int result = sscanf(line, "%lf %lf", &x, &y);

        // Check how many values were successfully parsed in each line, we're epecting 2 floating points; for anything else we're skipping the scanned line
        if (result != 2) {
            fprintf(stderr, "Invalid input format found. Expected two doubles in line: '%s' - Skipping\n", line);
            continue;  // Skip execution for this line and continue to the next line
        }

        // Check for NaN values after reading
        if (isnan(x) || isnan(y)) {
            fprintf(stderr, "Invalid point detected: (%f, %f) - Skipping\n", x, y);
            continue;  // Skip execution for this line and continue to the next line
        }

        // Store valid point
        (*points)[count].x = x;
        (*points)[count].y = y;
        count++;

        // Reallocate if needed
        if (count % k == 0) {  // Reallocate more memory if needed
            Point *new_points = realloc(*points, (count + k) * sizeof(Point));
            if (new_points == NULL) {
                free(*points);  // Free the original memory if reallocation fails
                fprintf(stderr, "Memory allocation failed while reading points\n");
                exit(1);
            }
            *points = new_points;  // Update *points only if realloc succeeds
        }
    }

    fclose(file);
    return count; // Return the number of points read
}

// Function to initialize clusters by randomly selecting initial centroids
void initialize_random_clusters(Cluster clusters[], int k, Point points[], int num_points) {
    srand(time(NULL));
    for (int i = 0; i < k; i++) {
        // Selecting an index of array(of size) randomly that points to a point as a centroid
        int random_index = rand() % num_points;
        clusters[i].centroid = points[random_index];
        allocate_memory_cluster(clusters, num_points, i);
    }
}

// Function to initialize clusters according the selection of the user for initial centroids
void initialize_custom_clusters(Cluster clusters[], Point** centroids, int k, Point points[], int num_points) {
    for (int i = 0; i < k; i++) {
        clusters[i].centroid.x = centroids[i]->x;
        clusters[i].centroid.y = centroids[i]->y;
        allocate_memory_cluster(clusters, num_points, i);
    }
}


void allocate_memory_cluster(Cluster clusters[], int num_points, int i) {
    clusters[i].num_points = 0;
    clusters[i].points = malloc(num_points * sizeof(Point));

    if (clusters[i].points == NULL) {
        fprintf(stderr, "Memory allocation failed for cluster points\n");
        exit(1);
    }
}

// Function to get the euclidean distance between point a & b
double get_euclidean_distance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Function to assign points to the nearest cluster
void assign_points_to_clusters(Point points[], int num_points, Cluster clusters[], int k) {
    int updated_num_points = 0;
    for (int i = 0; i < num_points; i++) {
        double min_distance = INFINITY;
        int cluster_index = 0;

        // Find the nearest cluster centroid
        for (int j = 0; j < k; j++) {
            double dist = get_euclidean_distance(points[i], clusters[j].centroid);
            if (dist < min_distance) {
                min_distance = dist;
                cluster_index = j;
            }
        }

        // Assign the point to the closest cluster
        updated_num_points = clusters[cluster_index].num_points++;
        clusters[cluster_index].points[updated_num_points] = points[i];
    }
}

// Function to update the cluster centroids
void update_centroids(Cluster clusters[], int k) {
    for (int i = 0; i < k; i++) {
        double sum_x = 0, sum_y = 0;

        // Calculate the new centroid
        for (int j = 0; j < clusters[i].num_points; j++) {
            sum_x += clusters[i].points[j].x;
            sum_y += clusters[i].points[j].y;
        }

        if (clusters[i].num_points > 0) { // Avoiding division by zero
            int mean_x = sum_x / clusters[i].num_points;
            int mean_y = sum_y / clusters[i].num_points;
            // Update the centroids
            clusters[i].centroid.x = mean_x;
            clusters[i].centroid.y = mean_y;
        }
    }
}

// Function to check if the centroids have changed
bool centroids_changed(Cluster clusters[], Cluster old_clusters[], int k) {
    for (int i = 0; i < k; i++) {
        if (clusters[i].centroid.x != old_clusters[i].centroid.x ||
            clusters[i].centroid.y != old_clusters[i].centroid.y) {
            return true; // Centroids have changed
        }
    }
    return false; // No change in centroids
}

// Function to write the cluster assignments to a file
void write_clusters(const char *filename, Cluster clusters[], int k, Point points[], int num_points) {
    FILE *file = fopen(filename, "w"); // Creates the file if not already created
    if (!file) {
        perror("Failed to open output file"); // In case the file is not available and can't be created
        return;
    }
    
    for (int j = 0; j < num_points; j++) {
        // Loop through each cluster to find which one contains the point
        for (int i = 0; i < k; i++) {
            for (int l = 0; l < clusters[i].num_points; l++) {
                if (clusters[i].points[l].x == points[j].x && clusters[i].points[l].y == points[j].y) {
                    // Print the point's coordinates and the cluster it belongs to
                    fprintf(file, "%-7.2f %-7.2f %d\n", points[j].x, points[j].y, i + 1);
                    break;  // Exit the inner loop once the point's cluster is found
                }
            }
        }
    }

    fclose(file);
}