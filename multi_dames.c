#include <stdio.h>

// Initialisation des variable globales du programme
#define TAILLE 8
#define MAX_JOUEURS 4


// Initialisation des structures
typedef struct {
    int pion[TAILLE][TAILLE]; // 0 si vide, 1 si blanc, 2 si rouge et 3 si noir
} Plateau;

typedef struct {
    int etat; // 1 si dans la partie, 0 sinon
    int score;
} Joueur;

typedef struct {
    Plateau plateau;
    Joueur joueur[MAX_JOUEURS];
    int nb_joueurs;
    int joueur_courant;
    int tour;
    int pion_est_saisi; // 1 si un pion est saisi, 0 sinon
    int pion_i, pion_j; // la ligne et colonne du pion saisi (0 sinon)
} Jeu;


// Définition des fonctions
