#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  float x, y, z; // Position
  float r, g, b; // Color for the centroid
} Centroid;

typedef struct {
  float x, y, z;
} Sommet;

typedef struct {
  int v1, v2, v3; // Indices des sommets
} Face;

typedef struct {
  Sommet *sommets;
  Face *faces;
  int nb_sommets;
  int nb_faces;
} Maillage;

typedef struct {
  int v1, v2; // Indices des sommets de l'arête
  int face;   // Indice de la face associée (optionnel)
} Arete;

typedef struct {
  Centroid c1;
  Centroid c2;
} AreteDuale;

typedef struct NoeudAVL {
  Arete arete;
  int hauteur;
  struct NoeudAVL *gauche, *droite;
} NoeudAVL;

typedef struct {
  NoeudAVL *racine;
} ArbreAVL;

