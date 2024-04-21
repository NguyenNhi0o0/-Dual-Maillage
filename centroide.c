
#include "include.h"

// Initialiser un maillage
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

  free(centroids);
  freeMaillage(m);
  return 0;
}