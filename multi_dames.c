#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    // On vérifie si la case sélectionné est valide
    if (!(0 <= i && i < TAILLE) || !(0 <= j && j < TAILLE) || jeu->plateau.pion[i][j] == 0) {
        return 0;
    }
    // Si c'est le premier tour, il faut que le pion à capturer soit blanc
    if (jeu->tour == 0 && jeu->plateau.pion[i][j] != 1) {
        return 0;
    }

    // Ajout du score de la capture
    switch (jeu->plateau.pion[i][j])
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

    // Suppression du pion dans le plateau de jeu
    jeu->plateau.pion[i][j] = 0;

    // La capture c'est bien passé
    return 1;
}

// Vérifie si le pion sélectionné peut faire au moins 1 saut et renvoie les coordonnées possible
int pos_saut_possible(Jeu *jeu, int pion_i, int pion_j, int indice_saut[][8]) {

    int saut_possible = 0;

    // Parcours des cases où l'on peut sauté
    for(int i = -2; i <= 2; i+=2) {
        for(int j = -2; j <= 2; j+=2) {
            if (0 <= pion_i + i && pion_i + i < TAILLE && 0 <= pion_j + j && pion_j + j < TAILLE) {
                // Si la case adjacente contient un pion et que la suivante est vide
                if (jeu->plateau.pion[pion_i + i][pion_j + j] == 0 && jeu->plateau.pion[pion_i + (i/2)][pion_j + (j/2)] != 0) {
                    // On save les coordonnées de saut dans la liste
                    indice_saut[0][saut_possible] = pion_i + i;
                    indice_saut[1][saut_possible] = pion_j + j;
                    saut_possible ++;
                }
            }  
        }
    }
    // Renvoie du nombre de saut possible
    return saut_possible;
}

// Utilisée en début de tour pour saisir le pion permettant de faire des saut (par le joueur courant)
int jeu_saisir_pion(Jeu *jeu, int i, int j)  {

    // Vérifie si la case contient un pion
    if (jeu->plateau.pion[i][j] == 0) {
        return 0;
    }

    // Vérifie si le pion peut faire au moins 1 saut
    int indice_saut[2][8];
    if (pos_saut_possible(jeu, i, j, indice_saut) == 0) {
        return 0;
    }

    // On indique dans la structure "jeu" qu'un pion a été saisie
    jeu->pion_est_saisi = 1;
    jeu->pion_i = i;
    jeu->pion_j = j;

    // La sélection est correcte et c'est bien passé
    return 1;
}

// Fait sauter le pion saisi vers la case (vide) en position (i,j) en capturant le pion entre les deux positions au passage
int jeu_sauter_vers(Jeu *jeu, int i, int j) {

    // On vérifie si la case sélectionné pour le saut est bien vide
    if (jeu->plateau.pion[i][j] != 0) {
        return 0;
    }
    // On vérifie si la case sélectionné pour le saut est bien une case adjacente
    if (!(-2 <= (i - jeu->pion_i) && (i - jeu->pion_i) <= 2 && -2 <= (j - jeu->pion_j) && (j - jeu->pion_j) <= 2)) {
        return 0;
    }
    if (((i - jeu->pion_i) + (j - jeu->pion_j)) % 2 != 0) {
        return 0;
    }

    // Repérage du pion capturé
    int pion_capture_i, pion_capture_j;
    pion_capture_i = jeu->pion_i + ((i - jeu->pion_i) / 2);
    pion_capture_j = jeu->pion_j + ((j - jeu->pion_j) / 2);

    if (!jeu_capturer(jeu, pion_capture_i, pion_capture_j)) {
        return 0;
    }

    // Déplacement du pion saisie
    jeu->plateau.pion[i][j] = jeu->plateau.pion[jeu->pion_i][jeu->pion_j];
    jeu->plateau.pion[jeu->pion_i][jeu->pion_j] = 0;

    // Changement de la position du pion saisie
    jeu->pion_i = i;
    jeu->pion_j = j;

    return 1;
}

// Utilisée en début de tour pour arrêter de jouer (tant que le joueur courant n’est pas le dernier joueur)
int jeu_arreter(Jeu *jeu) {

    // On parcours la liste des joueurs de la partie
    for(int i = 1; i < jeu->nb_joueurs; i++) {
        // On vérifie que le joueur suivant continue de jouer grâce à son état
        if (jeu->joueur[(jeu->joueur_courant + i) % jeu->nb_joueurs].etat == 1) {
            // Le joueur actuel s'arrête
            jeu->joueur[jeu->joueur_courant].etat = 0;
            // Le joueur actuel peut s'arrêté
            return 1;
        }
    }
    // Le joueur actuel ne peut pas s'arrêté
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

// Remplie la structure jeu en fonction des normes demandés dans l'énoncé
void jeu_charger(Jeu *jeu) {

    // Lecture nombre de joureur, tour actuel et joueur courant
    scanf(" %d %d %d",&(jeu->nb_joueurs), &(jeu->tour), &(jeu->joueur_courant));

    // Lecture de l'état et du score de chaque joueur
    for(int i = 0; i < jeu->nb_joueurs; i++) {
        scanf(" %d %d", &(jeu->joueur[i].etat),  &(jeu->joueur[i].score));
    }

    // Lecture pion_est_saisie, i, j
    scanf(" %d %d %d", &(jeu->pion_est_saisi), &(jeu->pion_i), &(jeu->pion_j));

    // Lecture des éléments du plateau
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

// Vérifie qu'il y a au moins 1 pion qui peut faire un saut
int aucun_saut_possible(Jeu *jeu) {

    // Liste necessaire pour la fonction "pos_saut_possible"
    int pos_saut[2][8];
    // Parcours de tout les pion du plateau
    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            // On regarde si il y a au moins 1 saut possible à partir du pion i j
            if (pos_saut_possible(jeu, i, j, pos_saut)) {
                // Il existe au moins un pion pouvant sauter
                return 1;
            }
        }
    }
    // Aucun saut possible
    return 0;
}

// Calcul et applique la pénalité au dernier joueur qui n'a pas pu faire de saut
void penalite_fin_partie(Jeu *jeu) {
    
    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            // Déduction des points de la pénalité sur le score du joueur courant
            switch (jeu->plateau.pion[i][j])
            {
            // Pion blanc qui vaut 1
            case 1:
                jeu->joueur[jeu->joueur_courant].score -= 1;
                break;
            // Pion rouge qui vaut 5
            case 2:
                jeu->joueur[jeu->joueur_courant].score -= 5;
                break;
            // Pion noir qui vaut 8
            case 3:
                jeu->joueur[jeu->joueur_courant].score -= 8;
                break;
            }
        }
    }
}

// Initialise le plateau de jeu
void init_plateau(Jeu *jeu) {

    srand(time(NULL));
    // Tableau avec le nombre de pion blanc, rouge et noir à placer
    int tab_couleur[3] = {34, 20, 10};
    int nb_rand;

    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            // Tirage du nombre aléatoire entre 0 et 3 (correspond au indice de tab_couleur avec 3 qui renvoie sur la case d'indice 0)
            nb_rand = rand() % 4;
            while (tab_couleur[nb_rand % 3] == 0) {
                nb_rand = rand() % 4;
            }
            // On place le pion
            jeu->plateau.pion[i][j] = (nb_rand % 3) + 1;
            // On résuit le nombre de pion à poser
            tab_couleur[nb_rand % 3] --;
        }
    }
}

// Initialise les données de la structure jeu
void init_jeu(Jeu *jeu) {

    int nb_joueur;

    init_plateau(jeu);
    // Peut être faire une fonction à part pour certain truc
    printf("Entrez le nombre de joueur (entre 2 et 4) : ");
    scanf("%d", &nb_joueur);
    jeu->nb_joueurs = nb_joueur;
    jeu->joueur_courant = 0;
    jeu->pion_est_saisi = 0;
    jeu->pion_i = 0;
    jeu->pion_j = 0;
    jeu->tour = 0;

    for(int i = 0; i < nb_joueur; i++) {
        jeu->joueur[i].etat = 1;
        jeu->joueur[i].score = 0;
    }
}

// Afffichage interface

void affiche_score(Jeu *jeu){
    switch (jeu->nb_joueurs){
        case 2: // 2 joueurs
            printf("Score:\n    J1  J2\n    %d  %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score);
            break;
        case 3: // 3 joueurs
            printf("Score:\n    J1  J2  J3\n    %d  %d  %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score,
                jeu->joueur[2].score);
            break;
        default:
            printf("Score:\n    J1  J2  J3  J4\n    %d  %d  %d  %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score,
                jeu->joueur[2].score,
                jeu->joueur[3].score);
    }
}

void affiche_tour(Jeu *jeu){
    printf("Tour: %d\n", jeu->tour);
}

void affiche_joueur_courant(Jeu *jeu){
    printf("Joueur %d (%d)\n",jeu->joueur_courant ,jeu->joueur[jeu->joueur_courant].score);
}

// Choix unique au premier tour
void affiche_anchoix(void){
    printf("Entrer la position d'un pion blanc (sous la forme : ligne colonne)\n");
}

// choix global hors 1er jusqu'a la fin
void affiche_choix(void){
    printf("Entrer la position d'un pion de départ\n");
}

void affiche_arret(void){
    printf("Arrêter ? (1 pour oui, 0 pour non)\n");
}

// Ne pas oublier la fonction des sauts possibles sinon, la mort
void affiche_sauts_possibles(int taille, int possibilites[2][taille]){
    printf("Sauts possibles: ");
    printf("%d %d", possibilites[0][1] ,possibilites[0][1]);
    for(int li = 1; li < taille; li++){
        printf(", %d %d",possibilites[li][0] ,possibilites[li][1]);
    }
    printf(".\n");
}

void affiche_plateau(Jeu *jeu){
    printf("   "); for(int i = 1; i <= TAILLE; i++){printf(" %d", i);} printf("\n");
    
    printf("   "); for(int i = 1; i <= TAILLE; i++){printf("%c", '_');printf("%c", '_');} printf("\n");
    
    for(int li = 0; li < TAILLE; li++){
        printf("%d  |", li+1);
        for(int col = 0; col < TAILLE; col++){
            switch(jeu->plateau.pion[li][col]){
                case 1: printf("+ "); break;
                case 2: printf("o "); break;
                case 3: printf("x "); break;
                default:printf("  ");
            }
        }
        printf("\n");
    }
}


int main(void) {

    Jeu jeu;
    init_jeu(&jeu);
    affiche_score(&jeu);
    affiche_tour(&jeu);
    affiche_joueur_courant(&jeu);
    affiche_plateau(&jeu);
    return 0;
}