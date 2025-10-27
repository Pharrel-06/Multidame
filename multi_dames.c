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


// Le joueur courant capture la pièce se trouvant à la ligne i et la colonne j
int jeu_capturer(Jeu *jeu, int i, int j) {

    // Suppression du pion dans le plateau de jeu
    jeu->plateau.pion[i][j] = 0;
    // Augmentation du score du joueur en question
    jeu->joueur[jeu->joueur_courant].score ++;

    return 0;
}

// Utilisée en début de tour pour saisir le pion permettant de faire des saut (par le joueur courant)
int jeu_saisir_pion(Jeu *jeu, int i, int j)  {

    // On indique dans la structure "jeu" qu'un pion a été saisie
    jeu->pion_est_saisi = 1;
    jeu->pion_i = i;
    jeu->pion_j = j;

    return 0;
}

// Fait sauter le pion saisi vers la case (vide) en position (i,j) en capturant le pion entre les deux positions au passage
int jeu_sauter_vers(Jeu *jeu, int i, int j) {

    // Repérage du pion capturé
    int pion_capture_i, pion_capture_j;
    pion_capture_i = (i - jeu->pion_i) / 2;
    pion_capture_j = (j - jeu->pion_j) / 2;

    // Ajout du score de la capture
    switch (jeu->plateau.pion[pion_capture_i][pion_capture_j])
    {
    case 1:
        jeu->joueur[jeu->joueur_courant].score += 1;
        break;
    case 2:
        jeu->joueur[jeu->joueur_courant].score += 5;
        break;
    case 3:
        jeu->joueur[jeu->joueur_courant].score += 8;
        break;
    }

    // Suppréssion du pion capturé
    jeu->plateau.pion[pion_capture_i][pion_capture_j] = 0;

    // Déplacement du pion saisie
    jeu->plateau.pion[i][j] = jeu->plateau.pion[jeu->pion_i][jeu->pion_j];

    // Changement de la position du pion saisie
    jeu->pion_i = i;
    jeu->pion_j = j;

    return 0;
}

// Utilisée en début de tour pour arrêter de jouer (tant que le joueur courant n’est pas le dernier joueur)
int jeu_arreter(Jeu *jeu) {

    return 0;
}

// Passe au joueur suivant
int jeu_joueur_suivant(Jeu *jeu) {

    jeu->joueur_courant = (jeu->joueur_courant + 1) % jeu->nb_joueurs;
    return 0;
}