
#include "include.h"

#include "include.h"


// Initialiser un maillage

// // Initialiser un maillage
Maillage *initMaillage(int nb_sommets, int nb_faces) {
  Maillage *m = malloc(sizeof(Maillage));
  m->sommets = malloc(sizeof(Sommet) * nb_sommets);
  m->faces = malloc(sizeof(Face) * nb_faces);
  m->nb_sommets = nb_sommets;
  m->nb_faces = nb_faces;
  return m;
}

// Libérer la mémoire allouée pour un maillage
void freeMaillage(Maillage *m) {
  free(m->sommets);
  free(m->faces);
  free(m);
}

Maillage *lireObj(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    perror("Erreur d'ouverture du fichier");
    return NULL;
  }

  char ligne[256];
  int nb_sommets = 0;
  int nb_faces = 0;

  while (fgets(ligne, sizeof(ligne), f)) {
    if (ligne[0] == 'v' && ligne[1] == ' ')
      nb_sommets++;
    else if (ligne[0] == 'f')
      nb_faces++;
  }

  Maillage *m = initMaillage(nb_sommets, nb_faces);

  rewind(f);

  int idx_sommet = 0;
  int idx_face = 0;

  while (fgets(ligne, sizeof(ligne), f)) {
    if (ligne[0] == 'v' && ligne[1] == ' ') {
      sscanf(ligne, "v %f %f %f", &(m->sommets[idx_sommet].x),
             &(m->sommets[idx_sommet].y), &(m->sommets[idx_sommet].z));
      idx_sommet++;
    } else if (ligne[0] == 'f') {
      sscanf(ligne, "f %d %d %d", &(m->faces[idx_face].v1),
             &(m->faces[idx_face].v2), &(m->faces[idx_face].v3));
      // Convertir les indices de 1-based à 0-based
      m->faces[idx_face].v1--;
      m->faces[idx_face].v2--;
      m->faces[idx_face].v3--;
      idx_face++;
    }
  }

  fclose(f);
  return m;
}

// Fonction pour calculer le centroïde d'une face
Centroid calculerCentroid(Maillage *m, Face f) {
  Centroid c;
  c.x = (m->sommets[f.v1].x + m->sommets[f.v2].x + m->sommets[f.v3].x) / 3.0;
  c.y = (m->sommets[f.v1].y + m->sommets[f.v2].y + m->sommets[f.v3].y) / 3.0;
  c.z = (m->sommets[f.v1].z + m->sommets[f.v2].z + m->sommets[f.v3].z) / 3.0;
  return c;
}

// Fonction pour générer un tableau de centroïdes
Centroid *genererCentroides(Maillage *m) {
  Centroid *centroids = malloc(sizeof(Centroid) * m->nb_faces);
  for (int i = 0; i < m->nb_faces; i++) {
    centroids[i] = calculerCentroid(m, m->faces[i]);
  }
  return centroids;
}

// Fonction pour vérifier si deux arêtes sont équivalentes
int sontEquivalentes(Arete a1, Arete a2) {
  return ((a1.v1 == a2.v1 && a1.v2 == a2.v2) ||
          (a1.v1 == a2.v2 && a1.v2 == a2.v1));
}

// Fonction pour établir un ordre entre deux arêtes
int estSuperieureA(Arete a1, Arete a2) {
  // Un exemple de comparaison basé sur les indices des sommets
  if (a1.v1 != a2.v1)
    return a1.v1 > a2.v1;
  return a1.v2 > a2.v2;
}

// verifier pour eviter la repetation
void ajouterAreteSiUnique(Arete *aretes, int *nb_aretes, Arete nouvelleArete) {
  for (int i = 0; i < *nb_aretes; i++) {
    if ((aretes[i].v1 == nouvelleArete.v1 &&
         aretes[i].v2 == nouvelleArete.v2) ||
        (aretes[i].v1 == nouvelleArete.v2 &&
         aretes[i].v2 == nouvelleArete.v1)) {
      return; // Arête déjà présente
    }
  }
  aretes[*nb_aretes] = nouvelleArete;
  (*nb_aretes)++;
}

void creerAretes(Maillage *m, Arete *aretes, int *nb_aretes) {
  for (int i = 0; i < m->nb_faces; i++) {
    for (int j = i + 1; j < m->nb_faces; j++) {
      Face faceA = m->faces[i];
      Face faceB = m->faces[j];
      int sommetsCommuns[2], idx = 0;

      // Trouver les sommets communs
      if (faceA.v1 == faceB.v1 || faceA.v1 == faceB.v2 || faceA.v1 == faceB.v3)
        sommetsCommuns[idx++] = faceA.v1;
      if (faceA.v2 == faceB.v1 || faceA.v2 == faceB.v2 || faceA.v2 == faceB.v3)
        sommetsCommuns[idx++] = faceA.v2;
      if (faceA.v3 == faceB.v1 || faceA.v3 == faceB.v2 || faceA.v3 == faceB.v3)
        sommetsCommuns[idx++] = faceA.v3;

      // Créer une arête si exactement 2 sommets sont communs
      if (idx == 2) {
        Arete ar = {sommetsCommuns[0], sommetsCommuns[1]};
        ajouterAreteSiUnique(aretes, nb_aretes, ar);
      }
    }
  }
}

// Cette fonction calcule une valeur représentative pour une arête en utilisant
// les coordonnées des centroïdes des faces que l'arête relie. Elle est utilisée
// pour trier les arêtes en fonction de la position de leurs centroïdes
// associés.

float valeurCentroide(Maillage *m, Arete ar) {
  Centroid c1 = calculerCentroid(m, m->faces[ar.v1]);
  Centroid c2 = calculerCentroid(m, m->faces[ar.v2]);
  return c1.x + c1.y + c1.z + c2.x + c2.y + c2.z;
}

Arete *trierAretesParSelection(Maillage *m, Arete *aretes, int nb_aretes) {
  // Créer un nouveau tableau pour le tri
  Arete *aretesTriees = malloc(sizeof(Arete) * nb_aretes);
  memcpy(aretesTriees, aretes, sizeof(Arete) * nb_aretes);

  // Trier les arêtes dans le nouveau tableau
  for (int i = 0; i < nb_aretes - 1; i++) {
    int min_idx = i;
    for (int j = i + 1; j < nb_aretes; j++) {
      if (valeurCentroide(m, aretesTriees[j]) <
          valeurCentroide(m, aretesTriees[min_idx])) {
        min_idx = j;
      }
    }
    // Échange
    Arete temp = aretesTriees[min_idx];
    aretesTriees[min_idx] = aretesTriees[i];
    aretesTriees[i] = temp;
  }
  // Retourner le nouveau tableau d'arêtes trié
  return aretesTriees;
}

void genererAretesDuales(Maillage *m, Arete *aretes, int nb_aretes,
                         AreteDuale *aretesDuales) {
  Arete *arretTri = trierAretesParSelection(m, aretes, nb_aretes);
  for (int i = 0; i < nb_aretes; i++) {
    aretesDuales[i].c1 = calculerCentroid(m, m->faces[arretTri[i].v1]);
    aretesDuales[i].c2 = calculerCentroid(m, m->faces[arretTri[i].v2]);
  }
}

void ecrireAretesDualesDansFichier(AreteDuale *aretesDuales, int nb_aretes,
                                   const char *nomFichier) {
  FILE *fichier = fopen(nomFichier, "w");
  if (fichier == NULL) {
    perror("Erreur lors de l'ouverture du fichier");
    return;
  }

  for (int i = 1; i <= nb_aretes; i++) {
    fprintf(fichier, "Arête duale %d: c1 (%f, %f, %f) c2 (%f, %f, %f)\n", i,
            aretesDuales[i].c1.x, aretesDuales[i].c1.y, aretesDuales[i].c1.z,
            aretesDuales[i].c2.x, aretesDuales[i].c2.y, aretesDuales[i].c2.z);
  }

  fclose(fichier);
}

void colorCentroidsBFS(Maillage *m) {
  int *visited = malloc(sizeof(int) * m->nb_faces);
  int *queue = malloc(sizeof(int) * m->nb_faces);
  int *distance = malloc(sizeof(int) * m->nb_faces);
  int front = 0, rear = 0, i;

  // Initialize arrays
  for (i = 0; i < m->nb_faces; i++) {
    visited[i] = 0;
    distance[i] = INT_MAX;
  }

  // Starting from the first centroid, which is black
  visited[0] = 1;
  distance[0] = 0;
  queue[rear++] = 0; // Enqueue the starting centroid

  while (front < rear) {
    int current = queue[front++]; // Dequeue a centroid

    // Process all adjacent centroids
    for (i = 0; i < m->nb_faces; i++) {
      // Check if there is an edge between current and i
      if (!visited[i] && areAdjacent(m, current, i)) {
        visited[i] = 1;
        distance[i] = distance[current] + 1;
        queue[rear++] = i; // Enqueue the adjacent centroid
      }
    }
  }

  // Find the maximum distance to normalize colors
  int max_distance = 0;
  for (i = 0; i < m->nb_faces; i++) {
    if (distance[i] > max_distance) max_distance = distance[i];
  }

  // Assign colors based on distance
  for (i = 0; i < m->nb_faces; i++) {
    float intensity = (max_distance == 0) ? 0.0f : (float)distance[i] / max_distance;
    m->Centroid[i].r = intensity;
    m->Centroid[i].g = intensity;
    m->Centroid[i].b = intensity;
  }

  free(visited);
  free(queue);
  free(distance);
}



int main() {
  Maillage *m = lireObj("maillages/cube.obj");
  if (!m) {
    printf("Erreur de lecture du fichier obj.\n");
    return 1;
  }

  // Affichage de test
  printf("nb_sommet %d \nnb_face %d \n", m->nb_sommets, m->nb_faces);

  Centroid *centroids = genererCentroides(m);
  printf("Centroides:\n");
  for (int i = 1; i <= m->nb_faces; i++) {
    printf("Centroide %d: %f %f %f\n", i, centroids[i].x, centroids[i].y,
           centroids[i].z);
  }

  Arete *aretes = malloc(sizeof(Arete) * m->nb_faces *
                         3); // Pire cas: chaque face a des arêtes uniques
  int nb_aretes = 0;

  creerAretes(m, aretes, &nb_aretes);

  // trierAretesParSelection(m, aretes, nb_aretes);

  clock_t debut, fin;
  double temps_ecoule;

  // Générer les arêtes duales
  AreteDuale aretesDuales[nb_aretes];
  debut = clock();
  genererAretesDuales(m, aretes, nb_aretes, aretesDuales);
  fin = clock();
  temps_ecoule = ((double)(fin - debut)) / CLOCKS_PER_SEC;
  printf("Temps d'exécution pour %d arêtes : %f secondes\n", nb_aretes,
         temps_ecoule);

  // Écrire les arêtes duales dans un fichier
  const char *nomFichierSortie = "triParSelection.obj";
  ecrireAretesDualesDansFichier(aretesDuales, nb_aretes, nomFichierSortie);

  free(aretes);
  free(centroids);
  freeMaillage(m);
  return 0;
}