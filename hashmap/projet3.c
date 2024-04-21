#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hashmap/hashmap.h"

// Structure pour représenter un sommet
typedef struct {
    float x, y, z;
} Vertex;

// Structure pour représenter une face
typedef struct {
    int v1, v2, v3; // Indices des sommets
} Face;

// Structure pour représenter un maillage
typedef struct {
    Vertex *vertices; // Tableau des sommets
    int numVertices;  // Nombre total de sommets

    Face *faces;      // Tableau des faces
    int numFaces;     // Nombre total de faces
} Mesh;

// Structure pour représenter une arête duale
typedef struct {
    int indiceCentroide1;
    int indiceCentroide2;
    int indiceFaceAssociee; // Si nécessaire
    // Ajoutez d'autres informations si besoin
} Arete;


// Fonction pour déterminer si deux arêtes sont équivalentes
int sontEquivalentes(Arete a1, Arete a2) {
    return (a1.indiceCentroide1 == a2.indiceCentroide1 && a1.indiceCentroide2 == a2.indiceCentroide2) ||
           (a1.indiceCentroide1 == a2.indiceCentroide2 && a1.indiceCentroide2 == a2.indiceCentroide1);
}

// Fonction pour déterminer si une arête est supérieure à une autre (à définir selon vos critères)
int estSuperieureA(Arete a1, Arete a2) {
    // À définir selon votre logique
    return 0;
}



// Fonction pour lire un fichier .obj et remplir la structure Maillage
Mesh* lireObj(const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    Mesh* maillage = malloc(sizeof(Mesh));
    maillage->numVertices = 0;
    maillage->numFaces = 0;

    while (!feof(fichier)) {
        char ligne[256];
        fgets(ligne, sizeof(ligne), fichier);

        if (ligne[0] == 'v') {
            maillage->numVertices++;
            maillage->vertices = realloc(maillage->vertices, maillage->numVertices * sizeof(Vertex));

            sscanf(ligne, "v %f %f %f", &maillage->vertices[maillage->numVertices - 1].x,
                                         &maillage->vertices[maillage->numVertices - 1].y,
                                         &maillage->vertices[maillage->numVertices - 1].z);
        } else if (ligne[0] == 'f') {
            maillage->numFaces++;
            maillage->faces = realloc(maillage->faces, maillage->numFaces * sizeof(Face));

            sscanf(ligne, "f %d %d %d", &maillage->faces[maillage->numFaces - 1].v1,
                                        &maillage->faces[maillage->numFaces - 1].v2,
                                        &maillage->faces[maillage->numFaces - 1].v3);
        }
    }

    fclose(fichier);
    return maillage;
}

// Fonction pour libérer la mémoire allouée pour le maillage
void libererMaillage(Mesh* maillage) {
    free(maillage->vertices);
    free(maillage->faces);
    free(maillage);
}



// Fonction pour écrire un graphe dual dans un fichier .obj
void ecrireGrapheDual(const char* nomFichier, Vertex* centroides, int* aretes, int numCentroides, int numAretes) {
    FILE* fichier = fopen(nomFichier, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Écrire les sommets (centroïdes)
    for (int i = 0; i < numCentroides; i++) {
        fprintf(fichier, "v %f %f %f\n", centroides[i].x, centroides[i].y, centroides[i].z);
    }

    // Écrire les arêtes du graphe dual
    for (int i = 0; i < numAretes; i += 2) {
        fprintf(fichier, "l %d %d\n", aretes[i], aretes[i + 1]);
    }

    fclose(fichier);
}


// Fonction pour générer un tableau de centroïdes à partir du maillage
Vertex* genererCentroides(Mesh* maillage) {
    Vertex* centroides = malloc(maillage->numFaces * sizeof(Vertex));

    for (int i = 0; i < maillage->numFaces; i++) {
        Face currentFace = maillage->faces[i];
        Vertex centroid;
        centroid.x = (maillage->vertices[currentFace.v1 - 1].x +
                      maillage->vertices[currentFace.v2 - 1].x +
                      maillage->vertices[currentFace.v3 - 1].x) / 3.0;

        centroid.y = (maillage->vertices[currentFace.v1 - 1].y +
                      maillage->vertices[currentFace.v2 - 1].y +
                      maillage->vertices[currentFace.v3 - 1].y) / 3.0;

        centroid.z = (maillage->vertices[currentFace.v1 - 1].z +
                      maillage->vertices[currentFace.v2 - 1].z +
                      maillage->vertices[currentFace.v3 - 1].z) / 3.0;

        centroides[i] = centroid;
    }

    return centroides;
}

// Fonction de tri par sélection pour les arêtes duales
void triParSelection(Arete* aretes, int nbAretes) {
    for (int i = 0; i < nbAretes - 1; i++) {
        int indiceMin = i;
        for (int j = i + 1; j < nbAretes; j++) {
            if (estSuperieureA(aretes[j], aretes[indiceMin])) {
                indiceMin = j;
            }
        }
        // Échange des arêtes
        Arete temp = aretes[i];
        aretes[i] = aretes[indiceMin];
        aretes[indiceMin] = temp;
    }
}

// Fonction pour générer les arêtes duales à partir du tableau trié (tri par sélection)
void genererAretesDualesTriSelection(Vertex* centroides, Arete* aretes, int nbAretes) {
    // Tri par sélection des arêtes
    triParSelection(aretes, nbAretes);

    // Génération des arêtes duales (à adapter selon vos besoins)
    // ...
}



// Fonction pour ajuster un tas
void ajusterTas(Arete* aretes, int debut, int fin) {
    int racine = debut;
    while ((2 * racine) + 1 <= fin) {
        int enfant = (2 * racine) + 1;
        if (enfant + 1 <= fin && estSuperieureA(aretes[enfant], aretes[enfant + 1])) {
            enfant = enfant + 1;
        }
        if (estSuperieureA(aretes[racine], aretes[enfant])) {
            Arete temp = aretes[racine];
            aretes[racine] = aretes[enfant];
            aretes[enfant] = temp;
            racine = enfant;
        } else {
            break;
        }
    }
}

// Fonction pour trier par tas les arêtes du maillage
void triParTas(Arete* aretes, int nbAretes) {
    for (int i = nbAretes / 2 - 1; i >= 0; i--) {
        ajusterTas(aretes, i, nbAretes - 1);
    }
    for (int i = nbAretes - 1; i >= 1; i--) {
        Arete temp = aretes[0];
        aretes[0] = aretes[i];
        aretes[i] = temp;
        ajusterTas(aretes, 0, i - 1);
    }
}

// Fonction pour générer les arêtes duales à partir du tableau trié (tri par tas)
void genererAretesDualesTriTas(Vertex* centroides, Arete* aretes, int nbAretes) {
    // Tri par tas des arêtes
    triParTas(aretes, nbAretes);

    // Génération des arêtes duales (à adapter selon vos besoins)
    // ...
}


// Structure pour représenter un nœud d'un arbre AVL d'arêtes
typedef struct NoeudAVL {
    Arete arete;
    struct NoeudAVL* gauche;
    struct NoeudAVL* droit;
    int hauteur;
} NoeudAVL;

// Structure pour représenter un arbre AVL d'arêtes
typedef struct {
    NoeudAVL* racine;
} ArbreAVL;

// Fonction pour calculer la hauteur d'un nœud
int hauteur(NoeudAVL* n) {
    if (n == NULL) {
        return 0;
    }
    return n->hauteur;
}

// Fonction pour calculer l'équilibre d'un nœud
int equilibre(NoeudAVL* n) {
    if (n == NULL) {
        return 0;
    }
    return hauteur(n->gauche) - hauteur(n->droit);
}

// Fonction pour effectuer une rotation simple à gauche
NoeudAVL* rotationGauche(NoeudAVL* y) {
    NoeudAVL* x = y->droit;
    NoeudAVL* T2 = x->gauche;

    // Rotation
    x->gauche = y;
    y->droit = T2;

    // Mise à jour des hauteurs
    y->hauteur = 1 + fmax(hauteur(y->gauche), hauteur(y->droit));
    x->hauteur = 1 + fmax(hauteur(x->gauche), hauteur(x->droit));

    return x;
}

// Fonction pour effectuer une rotation simple à droite
NoeudAVL* rotationDroite(NoeudAVL* x) {
    NoeudAVL* y = x->gauche;
    NoeudAVL* T2 = y->droit;

    // Rotation
    y->droit = x;
    x->gauche = T2;

    // Mise à jour des hauteurs
    x->hauteur = 1 + fmax(hauteur(x->gauche), hauteur(x->droit));
    y->hauteur = 1 + fmax(hauteur(y->gauche), hauteur(y->droit));

    return y;
}

// Fonction pour insérer une arête dans l'arbre AVL
NoeudAVL* insererAVL(NoeudAVL* racine, Arete arete) {
    // Insertion standard dans un arbre binaire de recherche
    if (racine == NULL) {
        NoeudAVL* nouveauNoeud = malloc(sizeof(NoeudAVL));
        nouveauNoeud->arete = arete;
        nouveauNoeud->gauche = NULL;
        nouveauNoeud->droit = NULL;
        nouveauNoeud->hauteur = 1;
        return nouveauNoeud;
    }

    if (estSuperieureA(arete, racine->arete)) {
        racine->droit = insererAVL(racine->droit, arete);
    } else {
        racine->gauche = insererAVL(racine->gauche, arete);
    }

    // Mise à jour de la hauteur du nœud actuel
    racine->hauteur = 1 + fmax(hauteur(racine->gauche), hauteur(racine->droit));

    // Rééquilibrage de l'arbre
    int facteurEquilibre = equilibre(racine);

    // Cas Gauche-Gauche
    if (facteurEquilibre > 1 && estSuperieureA(arete, racine->gauche->arete)) {
        return rotationDroite(racine);
    }

    // Cas Droit-Droit
    if (facteurEquilibre < -1 && estSuperieureA(racine->droit->arete, arete)) {
        return rotationGauche(racine);
    }

    // Cas Gauche-Droite
    if (facteurEquilibre > 1 && estSuperieureA(racine->gauche->arete, arete)) {
        racine->gauche = rotationGauche(racine->gauche);
        return rotationDroite(racine);
    }

    // Cas Droit-Gauche
    if (facteurEquilibre < -1 && estSuperieureA(arete, racine->droit->arete)) {
        racine->droit = rotationDroite(racine->droit);
        return rotationGauche(racine);
    }

    return racine;
}

// Fonction pour insérer une arête dans l'arbre AVL (version publique)
void insererArbreAVL(ArbreAVL* arbre, Arete arete) {
    arbre->racine = insererAVL(arbre->racine, arete);
}

// Fonction pour générer les arêtes duales à partir d'un AVL d'arêtes
void genererAretesDualesAVL(NoeudAVL* racine, Vertex* centroides) {
    if (racine != NULL) {
        // Parcours en ordre (gauche, racine, droit)
        genererAretesDualesAVL(racine->gauche, centroides);

        // Générer l'arête duale correspondante (à adapter selon vos besoins)
        Arete areteDuale;
        // Exemple : La face associée à l'arête duale pourrait être stockée dans areteDuale.indiceFaceAssociee

        // Insérer votre logique pour générer l'arête duale à partir du nœud de l'arbre AVL

        // Affichage (à adapter selon vos besoins)
        printf("Arête duale générée : (%d, %d)\n", areteDuale.indiceCentroide1, areteDuale.indiceCentroide2);

        // Générer d'autres arêtes duales ou effectuer d'autres opérations selon vos besoins

        genererAretesDualesAVL(racine->droit, centroides);
    }
}


#ifndef HASH_TABLE_H
#define HASH_TABLE_H

// Inclure les fichiers nécessaires de la bibliothèque hashmap.h

// Structure pour représenter une arête duale
typedef struct {
    // Définir les champs nécessaires pour représenter une arête duale
    // Par exemple, vous pouvez avoir des champs pour les sommets associés à l'arête
    // et toute autre information pertinente.
} DualEdge;

// Fonction pour créer une clé de hachage à partir d'une arête
unsigned int hash_key_from_dual_edge(const DualEdge *edge);

// Fonction pour créer une table de hachage et y insérer une arête duale
struct hashmap_s *create_and_insert_dual_edge_table(const DualEdge *edge);

// Fonction pour rechercher une arête duale dans la table de hachage
DualEdge *search_dual_edge_in_table(const struct hashmap_s *map, const DualEdge *edge);

#endif // HASH_TABLE_H




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hash_table.h" // Assurez-vous d'inclure le bon chemin vers votre fichier hash_table.h

// ... (les autres parties du code)

int main() {
    // Lire le fichier .obj pour obtenir le maillage
    Mesh* maillage = lireObj("bunny.obj");

    // Générer les centroides à partir du maillage
    Vertex* centroides = genererCentroides(maillage);

    // Générer les arêtes duales (utilisez la méthode de votre choix : tri par sélection, tri par tas, arbre AVL, etc.)
    int nbAretes = maillage->numFaces * 3; // Supposons que chaque face a trois arêtes
    Arete* aretes = malloc(nbAretes * sizeof(Arete));

    // Remplir le tableau 'aretes' avec les arêtes duales (exemple avec tri par sélection)
    genererAretesDualesTriSelection(centroides, aretes, nbAretes);

    // Créer la table de hachage pour les arêtes duales
    struct hashmap_s *table = hashmap_new();

    // Insérer les arêtes duales dans la table de hachage
    for (int i = 0; i < nbAretes; i++) {
        struct DualEdge dualEdge; // Assurez-vous de définir correctement la structure DualEdge
        // Initialisez la structure DualEdge avec les informations nécessaires à partir de 'aretes[i]'
        unsigned int key = hash_key_from_dual_edge(&dualEdge);
        hashmap_put(table, key, &dualEdge);
    }

    // Rechercher une arête duale dans la table de hachage (exemple avec la première arête)
    struct DualEdge searchEdge; // Initialisez la structure avec les informations nécessaires
    unsigned int searchKey = hash_key_from_dual_edge(&searchEdge);
    struct DualEdge *foundEdge = search_dual_edge_in_table(table, &searchEdge);

    // Vérifier si l'arête duale a été trouvée
    if (foundEdge != NULL) {
        printf("Arête duale trouvée dans la table de hachage.\n");
        // Faire quelque chose avec l'arête trouvée
    } else {
        printf("Arête duale non trouvée dans la table de hachage.\n");
    }

    // Libérer la mémoire allouée
    free(aretes);
    libererMaillage(maillage);
    hashmap_free(table); // Assurez-vous que votre bibliothèque hashmap.h dispose d'une fonction de libération appropriée

    return 0;
}
