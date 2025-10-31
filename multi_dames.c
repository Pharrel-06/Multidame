///////////////////////////////////////////////////////// Multi Dame /////////////////////////////////////////////////////////

// Import des bibliothèques
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
    if (jeu->plateau.pion[i][j] == 0) {
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

    // On vérifie si la case sélectionné est valide
    if (!(0 <= i && i < TAILLE) || !(0 <= j && j < TAILLE) || jeu->plateau.pion[i][j] == 0) {
        return 0;
    }

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
    // Liste avec les vecteurs de déplacement valide du pion saisir vers une case pour un saut
    int lst_adjacente_vide[2][8] = {{-2, -2, 0, 2, 2, 2, 0, -2},{0, 2, 2, 2, 0, -2, -2, -2}};
    int case_in_lst = 0;
    for(int n = 0; n < 8; n++) {
        if ((jeu->pion_i + lst_adjacente_vide[0][n]) == i && (jeu->pion_j + lst_adjacente_vide[1][n]) == j) {
            case_in_lst = 1;
        }
    }
    if (case_in_lst == 0) {
        return 0;
    }

    // Repérage du pion capturé
    int pion_capture_i, pion_capture_j;
    pion_capture_i = jeu->pion_i + ((i - jeu->pion_i) / 2);
    pion_capture_j = jeu->pion_j + ((j - jeu->pion_j) / 2);

    // On vérifie si il y a bien un pion à capturer, si oui on le capture
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
            // On vérifie que la case contient bien un pion :(
            if (jeu->plateau.pion[i][j] == 0){
                continue;
            }
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
    
    // Parcour du plateau
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
    
    printf("Entrez le nombre de joueur (entre 2 et 4) : ");
    scanf("%d", &nb_joueur);

    init_plateau(jeu);

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

// Affiche le score de tout les joueurs
void affiche_score(Jeu *jeu){
    switch (jeu->nb_joueurs){
        case 2: // 2 joueurs
            printf("Score:\n    J1  J2\n    %d   %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score);
            break;
        case 3: // 3 joueurs
            printf("Score:\n    J1  J2  J3\n    %d   %d   %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score,
                jeu->joueur[2].score);
            break;
        default:
            printf("Score:\n    J1  J2  J3  J4\n    %d   %d   %d   %d\n",
                jeu->joueur[0].score,
                jeu->joueur[1].score,
                jeu->joueur[2].score,
                jeu->joueur[3].score);
    }
}

// Affiche le nombre de tour
void affiche_tour(Jeu *jeu){
    printf("Tour: %d\n", jeu->tour + 1);
}

// Affiche le numéro du joueur courant
void affiche_joueur_courant(Jeu *jeu){
    printf("Joueur %d (%d)\n",jeu->joueur_courant + 1,jeu->joueur[jeu->joueur_courant].score);
}

// Affiche un texte pour la capture d'un pion blanc (pour le premier tour)
void affiche_anchoix(void){
    printf("Entrer la position d'un pion blanc (sous la forme : ligne colonne)\n");
}

// Affiche un texte pour saisir un pion
void affiche_choix(void){
    printf("Entrer la position d'un pion de départ (sous la forme : ligne colonne)\n");
}

// Affiche un texte pour demander l'arrêt du joueur
void affiche_arret(void){
    printf("Arrêter ? (1 pour oui, 0 pour non)\n");
}

// Affiche les coordonnées des cases disponible pour un saut 
void affiche_sauts_possibles(int taille, int possibilites[2][8]){
    if (taille > 0) {
        printf("Sauts possibles: ");
        printf("%d %d", possibilites[0][0] + 1, possibilites[1][0] + 1);
        for(int i = 1; i < taille; i++){
            printf(", %d %d", possibilites[0][i] + 1, possibilites[1][i] + 1);
        }
        printf(".\n");
    }
}

// Affiche le plateau de jeu
void affiche_plateau(Jeu *jeu){
    
    // Affichage des numéros de colonnes
    printf("   "); for(int i = 1; i <= TAILLE; i++){ printf(" %2d", i); } printf("\n");
    // Affichage d'une ligne de sépration 
    printf("   "); for(int i = 1; i <= TAILLE; i++){ printf("___"); } printf("\n");

    // Affichage ligne par ligne
    for(int li = 0; li < TAILLE; li++){
        // Affichage des numéros de lignes
        printf("%2d |", li+1);
        for(int col = 0; col < TAILLE; col++){
            // Si un pion est saisi et que c'est la case sélectionnée, on entoure par des crochets
            if (jeu->pion_est_saisi && li == jeu->pion_i && col == jeu->pion_j) {
                switch(jeu->plateau.pion[li][col]){
                    case 1: printf("[+]"); break;
                    case 2: printf("[o]"); break;
                    case 3: printf("[x]"); break;
                    default:printf("[ ]");
                }
            } else {
                switch(jeu->plateau.pion[li][col]){
                    case 1: printf(" + "); break;
                    case 2: printf(" o "); break;
                    case 3: printf(" x "); break;
                    default:printf("   ");
                }
            }
        }
        printf("\n");
    }
}

// Affiche les score de la partie, le nombre de tour, le joueur actuel et le plateau
void affiche_global(Jeu *jeu){
    affiche_score(jeu);
    affiche_tour(jeu);
    affiche_joueur_courant(jeu);
    affiche_plateau(jeu);
}

// Initialise le plateau de jeu et affiche les données de la partie
void initialise_jeu_base(Jeu *jeu){
    init_jeu(jeu);
    affiche_global(jeu);
}

// Représente la première partie du jeu dans laquelle chaque joueur doit capturer un pion blanc
void choix_premier_tour(Jeu *jeu){

    // Parcours des joueurs
    for(int i = 0; i < jeu->nb_joueurs; i++){
        // Réccupération du choix de l'utilisateur
        int choix_i, choix_j;
        affiche_anchoix();
        scanf("%d %d", &choix_i, &choix_j);
        // Vérification selon les règles
        while(!(jeu_capturer(jeu, choix_i - 1, choix_j - 1))){
            printf("Pion invalide, veuillez choisir un pion blanc.\n");
            affiche_anchoix();
            scanf("%d %d", &choix_i, &choix_j);
        }
        jeu_joueur_suivant(jeu);
        affiche_global(jeu);
    }
}

// Vérifie si la case de saut est bien compris dans la liste
int saut_in_liste(int i, int j, int liste[][8], int taille){
    for(int n = 0; n < taille; n++){
        if(liste[0][n] == i && liste[1][n] == j){
            return 1;
        }
    }
    return 0;
}

// Demande si l'utilisateur veut s'arrêter et vérifie si il peut le faire
int choix_arret_joueur(Jeu *jeu){

    // Réccupération du choix de l'utilisateur
    int arret;
    affiche_arret(); scanf("%d", &arret);

    // Si il veut s'arrêter :
    if (arret == 1){
        // On regarde si il peut s'arrêter
        if (jeu_arreter(jeu) == 1){
            printf("\nLe joueur %d s'arrete.\n\n", jeu->joueur_courant + 1);
            jeu_joueur_suivant(jeu);
            return 1;
        }else{
            printf("Vous etes le dernier joueur, continuer jusqu'a ne plus pouvoir capturer de pions.\n");
            return 0;
        }
    }
    return 0;
}

// Saisie le pion choisie par l'utilisateur
void choix_saisir_pion(Jeu *jeu){
    
    // Réccupération du choix de l'utilisateur
    int choix_i, choix_j;
    do{
        affiche_choix(); 
        scanf("%d %d",&choix_i, &choix_j);
        // On fait -1 pour passé des lignes de l'affichage aux lignes du tableau
        choix_i --; choix_j --;
    // Vérification de la saisie du pion
    }while(jeu_saisir_pion(jeu, choix_i, choix_j) == 0);
}

// Affiche et sélectionne les sauts du pion saisie par le joueur
void choix_saut(Jeu *jeu){

    // Tableau 2 dimensions qui contient les coordonnées des où le pion peut sauter
    int possibilites[2][8];
    int nb_sauts_possibles;

    // Tant que le pion saisie peut effecter des sauts, on réccupère les coordonnées des cases valides et leurs nombre
    while ((nb_sauts_possibles = pos_saut_possible(jeu, jeu->pion_i, jeu->pion_j, possibilites)) != 0) {
        // On affiche les cases disponibles pour le saut
        int choix_saut_i, choix_saut_j;
        affiche_sauts_possibles(nb_sauts_possibles, possibilites);
        // On réccupère la case sélectionné
        scanf("%d %d", &choix_saut_i, &choix_saut_j);
        // On fait -1 pour passé des lignes de l'affichage aux lignes du tableau
        choix_saut_i --; choix_saut_j --;

        // Si la case correspond à celle disponible, on fait le saut
        if (saut_in_liste(choix_saut_i, choix_saut_j, possibilites, nb_sauts_possibles) == 1){
            jeu_sauter_vers(jeu, choix_saut_i, choix_saut_j);
            affiche_plateau(jeu);
        } else {
            printf("Choix invalide.\n");
        }
    }
}

// Représente les tours de la partie (sauf le premier) avec la demande d'arrêt, la saisie d'un pion et les sauts pour chaque joueur
void choix_tour(Jeu *jeu){

    // Tant qu'il y a au moins 1 pion sur le plateau qui peut faire un saut valide, on continue la partie
    while(aucun_saut_possible(jeu)){
        // Affiche des données de la partie
        affiche_global(jeu);

        // Si le joueur souhaite arrêter, on ne fais pas de saisie de pion et on met à jour son état
        if (choix_arret_joueur(jeu)){
            continue;
        }
        
        // Saisie d'un pion par l'utilisateur
        choix_saisir_pion(jeu);

        // Affichage du plateau
        affiche_plateau(jeu);

        // On effectue les sauts possible avec le pion saisie
        choix_saut(jeu);

        // On passe au joueur suivant
        printf("Fin du tour du Joueur %d.\n",jeu->joueur_courant + 1);
        jeu_joueur_suivant(jeu);
    }
}

// Renvoie le gagnant de la partie 
int gagnant(Jeu *jeu){

    int gagnant = 0;
    for(int i = 1; i < jeu->nb_joueurs; i++){
        if (jeu->joueur[gagnant].score < jeu->joueur[i].score){gagnant = i;}
    }
    return gagnant;
}

// Fonction principale qui compose l'entièreté du jeu "Multi Dame"
void multi_dame(void){

    // Initialisation de la structure et des données de la partie
    Jeu jeu;
    initialise_jeu_base(&jeu);
    // Capture d'un pion blanc au premier tour
    choix_premier_tour(&jeu);
    // Déroulement de la partie sur tout les autres tours
    choix_tour(&jeu);

    // Annonce des résultats de la partie
    printf("Plus de sauts possibles sur le plateau. Fin de la partie\n");
    printf("Penalité appliquer a Joueur %d", jeu.joueur_courant + 1);
    penalite_fin_partie(&jeu);
    printf("\n\n"); 
    affiche_score(&jeu);
    printf("\nLe gagnant est... Le joueur %d!!!!\n", gagnant(&jeu) + 1);
}

// Main du programme C
int main(void) {
    multi_dame();
    return 0;
}