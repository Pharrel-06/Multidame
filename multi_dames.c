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
    // Pion blanc qui vaut 1
    case 1:
        jeu->joueur[jeu->joueur_courant].score += 1;
        break;
    // Pion rouge qui vaut 5
    case 2:
        jeu->joueur[jeu->joueur_courant].score += 5;
        break;
    // Pion noir qui vaut 8
    case 3:
        jeu->joueur[jeu->joueur_courant].score += 8;
        break;
    }

    // Suppréssion du pion capturé
    jeu->plateau.pion[pion_capture_i][pion_capture_j] = 0;

    // Déplacement du pion saisie
    jeu->plateau.pion[i][j] = jeu->plateau.pion[jeu->pion_i][jeu->pion_j];
    jeu->plateau.pion[jeu->pion_i][jeu->pion_j] = 0;

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

    // On parcours la liste des joueurs de la partie
    for(int i = 1; i <= jeu->nb_joueurs; i++) {
        // On vérifie que le joueur suivant continue de jouer grâce à son état
        if (jeu->joueur[(jeu->joueur_courant + i) % jeu->nb_joueurs].etat == 1) {
            // On vérifie si on doit passé au tour suivant
            if ((jeu->joueur_courant + i) % jeu->nb_joueurs <= jeu->joueur_courant) {
                jeu->tour ++;
            }
            // On met à jour toutes les données
            jeu->joueur_courant = (jeu->joueur_courant + i) % jeu->nb_joueurs;
            jeu->pion_est_saisi = 0;
            jeu->pion_i = 0;
            jeu->pion_j = 0;
            // On a réussi à passé au joueur suivant
            return 1;
        }
    }
    // On n'a pas réussi à passé au joueur suivant
    return 0;
}

// Remplie la strcture jeu en fonction des normes demandés dans l'énoncé
void jeu_charger(Jeu *jeu) {

    // Lecture nombre de joureur, tour actuel et joueur courant
    scanf(" %d %d %d",&(jeu->nb_joueurs), &(jeu->tour), &(jeu->joueur_courant));

    // Lecture de l'état et du score de chaque joueur
    for(int i = 0; i < jeu->nb_joueurs; i++) {
        scanf(" %d %d", &(jeu->joueur[i].etat),  &(jeu->joueur[i].score));
    }

    // Lecture pion_est_saisie, i, j
    scanf(" %d %d %d", &(jeu->pion_est_saisi), &(jeu->pion_i), &(jeu->pion_j));

    // Lecture des élément du plateau
    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            scanf(" %d",&(jeu->plateau.pion[i][j]));
        }
    }
}

// Affiche les données de la partie en fonction des normes demandés dans l'énoncé
void jeu_ecrire(Jeu *jeu) {
    
    // Nombre de joueur, tour, joueur courant
    printf("%d %d %d\n", jeu->nb_joueurs, jeu->tour, jeu->joueur_courant);

    // Etat et score de chaque joueur
    for(int i = 0; i < jeu->nb_joueurs; i++) {
        printf("%d %d\n", jeu->joueur[i].etat,  jeu->joueur[i].score);
    }
    // Pion est saisie, i, j
    printf("%d %d %d\n", jeu->pion_est_saisi, jeu->pion_i, jeu->pion_j);

    // Affichage plateau
    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            printf("%d ", jeu->plateau.pion[i][j]);
        }
        printf("\n");
    }
}
