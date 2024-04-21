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

  // Compter le nombre de sommets et de faces
  // La fonction parcourt le fichier ligne par ligne.
  // Si une ligne commence par 'v' (et est suivie d'un espace), elle représente
  // un sommet. Si elle commence par 'f', elle représente une face. Les
  // compteurs sont mis à jour en conséquence.
  //   (la taille nécessaire pour creer un maillage)
  while (fgets(ligne, sizeof(ligne), f)) {
    if (ligne[0] == 'v' && ligne[1] == ' ')
      nb_sommets++;
    else if (ligne[0] == 'f')
      nb_faces++;
  }

  // Après avoir déterminé le nombre de sommets et de faces,
  // un maillage est initialisé avec la capacité nécessaire pour stocker ces
  // sommets et ces faces.
  Maillage *m = initMaillage(nb_sommets, nb_faces);

  // Rembobiner le fichier (tua lai)
  // La fonction rembobine le fichier pour le lire à nouveau.
  // Cela est nécessaire car nous allons à nouveau parcourir le fichier
  // pour extraire et stocker les données des sommets et des faces.
  rewind(f);

  int idx_sommet = 0;
  int idx_face = 0;

  // Le fichier est à nouveau parcouru ligne par ligne.
  // Pour chaque ligne représentant un sommet, les coordonnées x, y, et z sont
  // extraites et stockées. Pour chaque ligne représentant une face, les indices
  // des sommets sont extraits et ajustés pour être 0-based (car le format OBJ
  // est 1-based).
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

int main() {
  Maillage *m = lireObj("maillages/cube.obj");
  if (!m) {
    printf("Erreur de lecture du fichier obj.\n");
    return 1;
  }

  // Affichage de test
  printf("nb_sommet %d \nnb_face %d \n", m->nb_sommets, m->nb_faces);
  
//     printf("Sommets:\n");
//   for (int i = 0; i < m->nb_sommets; i++) {
//     printf("v %f %f %f\n", m->sommets[i].x, m->sommets[i].y, m->sommets[i].z);
//   }

//   printf("Faces:\n");

//   for (int i = 0; i < m->nb_faces; i++) {
//     printf("f %d %d %d\n", m->faces[i].v1 + 1, m->faces[i].v2 + 1,
//            m->faces[i].v3 +
//                1); // Convertir de 0-based à 1-based pour l'affichage
//   }

  freeMaillage(m);
  return 0;
}
