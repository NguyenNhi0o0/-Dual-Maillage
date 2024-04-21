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

int hauteurNoeudAVL(NoeudAVL *noeud) {
  if (noeud == NULL)
    return 0;
  return noeud->hauteur;
}

NoeudAVL *rotationDroite(NoeudAVL *y) {
  NoeudAVL *x = y->gauche;
  NoeudAVL *T2 = x->droite;

  // Effectuer la rotation
  x->droite = y;
  y->gauche = T2;

  // Mettre à jour les hauteurs
  y->hauteur = MAX(hauteurNoeudAVL(y->gauche), hauteurNoeudAVL(y->droite)) + 1;
  x->hauteur = MAX(hauteurNoeudAVL(x->gauche), hauteurNoeudAVL(x->droite)) + 1;

  // Retourner la nouvelle racine
  return x;
}


NoeudAVL *rotationGauche(NoeudAVL *x) {
  NoeudAVL *y = x->droite;
  NoeudAVL *T2 = y->gauche;

  // Effectuer la rotation
  y->gauche = x;
  x->droite = T2;

  // Mettre à jour les hauteurs
  x->hauteur = MAX(hauteurNoeudAVL(x->gauche), hauteurNoeudAVL(x->droite)) + 1;
  y->hauteur = MAX(hauteurNoeudAVL(y->gauche), hauteurNoeudAVL(y->droite)) + 1;

  // Retourner la nouvelle racine
  return y;

}

int comparerArete(Arete a1, Arete a2) {
  if (a1.v1 < a2.v1)
    return -1;
  if (a1.v1 > a2.v1)
    return 1;
  if (a1.v2 < a2.v2)
    return -1;
  if (a1.v2 > a2.v2)
    return 1;
  return 0;
}

NoeudAVL* equilibrerNoeudAVL(NoeudAVL *noeud) {
    if (noeud == NULL) return NULL;

    // Mise à jour de la hauteur du noeud
    noeud->hauteur = 1 + MAX(hauteurNoeudAVL(noeud->gauche), hauteurNoeudAVL(noeud->droite));

    // Calcul du facteur d'équilibre
    int balance = hauteurNoeudAVL(noeud->gauche) - hauteurNoeudAVL(noeud->droite);

    // Si le noeud est déséquilibré, alors il y a 4 cas

    // Cas gauche gauche
    if (balance > 1 && comparerArete(noeud->gauche->arete, noeud->arete) < 0) {
        return rotationDroite(noeud);
    }

    // Cas droite droite
    if (balance < -1 && comparerArete(noeud->droite->arete, noeud->arete) > 0) {
        return rotationGauche(noeud);
    }

    // Cas gauche droite
    if (balance > 1 && comparerArete(noeud->gauche->arete, noeud->arete) > 0) {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    // Cas droite gauche
    if (balance < -1 && comparerArete(noeud->droite->arete, noeud->arete) < 0) {
        noeud->droite = rotationDroite(noeud->droite);
        return rotationGauche(noeud);
    }

    // Si le noeud est déjà équilibré, pas besoin de le modifier
    return noeud;
}


NoeudAVL *insererNoeudAVL(NoeudAVL *noeud, Arete arete) {
  if (noeud == NULL) {
    NoeudAVL *nouveauNoeud = (NoeudAVL *)malloc(sizeof(NoeudAVL));
    nouveauNoeud->arete = arete;
    nouveauNoeud->gauche = NULL;
    nouveauNoeud->droite = NULL;
    nouveauNoeud->hauteur = 1;
    return nouveauNoeud;
  }

  if (comparerArete(arete, noeud->arete) < 0)
    noeud->gauche = insererNoeudAVL(noeud->gauche, arete);
  else if (comparerArete(arete, noeud->arete) > 0)
    noeud->droite = insererNoeudAVL(noeud->droite, arete);
  else // Les arêtes sont équivalentes : pas d'insertion
    return noeud;

  // Mise à jour de la hauteur du noeud courant
  noeud->hauteur =
      1 + MAX(hauteurNoeudAVL(noeud->gauche), hauteurNoeudAVL(noeud->droite));

  // Équilibrer le noeud si nécessaire
  return equilibrerNoeudAVL(noeud);
}

NoeudAVL *rechercherNoeudAVL(NoeudAVL *racine, Arete arete) {
  if (racine == NULL || comparerArete(racine->arete, arete) == 0)
    return racine;

  if (comparerArete(arete, racine->arete) < 0)
    return rechercherNoeudAVL(racine->gauche, arete);

  return rechercherNoeudAVL(racine->droite, arete);
}



void parcoursInfixe(NoeudAVL *noeud, Maillage *m, AreteDuale *aretesDuales,
                    int *index) {
  if (noeud != NULL) {
    parcoursInfixe(noeud->gauche, m, aretesDuales, index);

    // Logique de calcul des arêtes duales
    aretesDuales[*index].c1 = calculerCentroid(m, m->faces[noeud->arete.v1]);
    aretesDuales[*index].c2 = calculerCentroid(m, m->faces[noeud->arete.v2]);
    (*index)++;

    parcoursInfixe(noeud->droite, m, aretesDuales, index);
  }
}

void genererAretesDualesAVL(Maillage *m, ArbreAVL *arbre,
                            AreteDuale *aretesDuales) {
  int index = 0;
  parcoursInfixe(arbre->racine, m, aretesDuales, &index);
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

int main() {
  Maillage *m = lireObj("maillages/cube.obj");
  if (!m) {
    printf("Erreur de lecture du fichier obj.\n");
    return 1;
  }

  printf("nb_sommet %d \nnb_face %d \n", m->nb_sommets, m->nb_faces);

  // Initialiser l'arbre AVL
  ArbreAVL arbre;
  arbre.racine = NULL;

  // ajoute les arêtes dans l'arbre AVL
  // Insérer les arêtes dans l'arbre AVL
  for (int i = 0; i < m->nb_faces; i++) {
    Face face = m->faces[i];
    Arete aretes[3];

    // Créer les arêtes à partir de la face
    aretes[0] = (Arete){face.v1, face.v2};
    aretes[1] = (Arete){face.v2, face.v3};
    aretes[2] = (Arete){face.v3, face.v1};

    // Insérer chaque arête dans l'arbre AVL
    for (int j = 0; j < 3; j++) {
      if (rechercherNoeudAVL(arbre.racine, aretes[j]) ==
          NULL) { // Vérifier si l'arête existe déjà
        arbre.racine = insererNoeudAVL(arbre.racine, aretes[j]);
      }
    }
  }

  // Générer les arêtes duales en utilisant l'arbre AVL
  AreteDuale *aretesDuales =
      malloc(sizeof(AreteDuale) * m->nb_faces * 3); // Taille maximale possible
  if (aretesDuales == NULL) {
    perror("Erreur d'allocation mémoire pour les arêtes duales");
    freeMaillage(m);
    return 1;
  }

  clock_t debut, fin;
  debut = clock();
  genererAretesDualesAVL(m, &arbre, aretesDuales);
  fin = clock();

  double temps_ecoule = ((double)(fin - debut)) / CLOCKS_PER_SEC;
  printf("Temps d'exécution pour générer les arêtes duales : %f secondes\n",
         temps_ecoule);

  // Écrire les arêtes duales dans un fichier
  const char *nomFichierSortie = "aretesDualesAVL.obj";
  ecrireAretesDualesDansFichier(aretesDuales, m->nb_faces * 3,
                                nomFichierSortie);

  free(aretesDuales);
  freeMaillage(m);
  return 0;
}
