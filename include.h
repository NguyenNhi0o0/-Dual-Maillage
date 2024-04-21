#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  float x, y, z;
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

#define MAX(a, b) ((a) > (b) ? (a) : (b))



Maillage *initMaillage(int nb_sommets, int nb_faces);

void freeMaillage(Maillage *m);

Maillage *lireObj(const char *filename);

Centroid calculerCentroid(Maillage *m, Face f);

Centroid *genererCentroides(Maillage *m);

int sontEquivalentes(Arete a1, Arete a2);

int estSuperieureA(Arete a1, Arete a2);

void ajouterAreteSiUnique(Arete *aretes, int *nb_aretes, Arete nouvelleArete);

void creerAretes(Maillage *m, Arete *aretes, int *nb_aretes);

float valeurCentroide(Maillage *m, Arete ar);

Arete *trierAretesParSelection(Maillage *m, Arete *aretes, int nb_aretes);

// void genererAretesDuales(Maillage *m, Arete *aretes, int nb_aretes);

void genererAretesDuales(Maillage *m, Arete *aretes, int nb_aretes,
                         AreteDuale *aretesDuales);

void ecrireAretesDualesDansFichier(AreteDuale *aretesDuales, int nb_aretes,
                                   const char *nomFichier);
