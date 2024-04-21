#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x;
    float y;
    float v; 
} Centroide;

void write_dual_graph_to_obj(Centroide *centroide, int numCentroides, int (*arrets)[2], int numArrets, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    // Écrire les sommets
    for (int i = 0; i < numCentroides; i++) {
        fprintf(file, "v %f %f %f\n", centroide[i].x, centroide[i].y, centroide[i].v);
        // printf("%d", i);
    }

    // Écrire les arêtes
    for (int i = 0; i < numArrets; i++) {
        fprintf(file, "l %d %d\n", arrets[i][0] + 1, arrets[i][1] + 1); // +1 car les indices dans .obj commencent à 1
    }

    fclose(file);
}

int main() {
    Centroide centroides[] = {{1, 2, 0.1}, {3, 4, 0.2}, {5, 6, 0.3}};
    int arrets[][2] = {{0, 1}, {1, 2}};

    int numCentroides = sizeof(centroides) / sizeof(Centroide);
    // printf("%d", numCentroides);
    int numArrets = sizeof(arrets) / (2 * sizeof(int));

    write_dual_graph_to_obj(centroides, numCentroides, arrets, numArrets, "dual_graph.obj");

    return 0;
}
