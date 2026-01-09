#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

#include "jeu.h"


/* ---------------------------------------------
 *                    DEBUG
 * ---------------------------------------------
 * 
 *
 * DEBUG_MODE = 1  -> affichage des DEBUG_PRINT
 * DEBUG_MODE = 0  -> désactiver
 * 
 */

#define DEBUG_MODE 0

# if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
# else
    #define DEBUG_PRINT(...)
#endif

#define PRINT_MODE 0

# if PRINT_MODE
    #define PRINT_PRINT(...) printf(__VA_ARGS__)
# else
    #define PRINT_PRINT(...)
#endif


/* ---------------------------------------------
 *   FONCTIONS D'INITIALISATION / DESTRUCTION
 * ---------------------------------------------
 */

/* Initialisation du jeu */
Jeu* initJeu(bool joueurMachine) {
    Jeu* jeu = (Jeu*)malloc(sizeof(Jeu));

    for (int i = 0; i < 16; i++) {
        jeu->rouge[i] = 2;
        jeu->bleu[i] = 2;
        jeu->transparent[i] = 2;
    }

    jeu->score[0] = 0;
    jeu->score[1] = 0;

    jeu->joueurMachine = joueurMachine;
    jeu->joueurActuel = 0;

    jeu->nbCoups = 0;

    return jeu;
}


/* Libération du jeu */
void libererJeu(Jeu* jeu) {
    free(jeu);
}


/* Création d'un coup */
Coup* creerCoup(int trou, int couleur) {
    Coup* coup = (Coup*)malloc(sizeof(Coup));
    coup->trou = trou;
    coup->couleur = couleur;
    return coup;
}


/* Libération d'un coup */
void libererCoup(Coup* coup) {
    free(coup);
}


/* ---------------------------------------------
 *    OUTILS SUR LE JOUEUR / LES TROUS
 * ---------------------------------------------
 */

/* Retourne l'indice de l'adversaire */
bool donnerAdversaire(Jeu* jeu) {
    return !jeu->joueurActuel;
}


/* Ajoute une graine dans un trou */
void ajouterGraine(Jeu* jeu, int trou, int couleur) {
    if (couleur == 0) {
        jeu->rouge[trou]++;
    } else if (couleur == 1) {
        jeu->bleu[trou]++;
    } else {
        jeu->transparent[trou]++;
    }
}


/**
 * Met à 0 toutes les graines d'une couleur dans un trou.
 * Retourne le nombre de graines prises.
 */
int prendreGraines(Jeu* jeu, int trou, int couleur) {
    int graines = 0;
    if (couleur == 0) {
        graines = jeu->rouge[trou];
        jeu->rouge[trou] = 0;
    } else if (couleur == 1) {
        graines = jeu->bleu[trou];
        jeu->bleu[trou] = 0;
    } else {
        graines = jeu->transparent[trou];
        jeu->transparent[trou] = 0;
    }
    return graines;
}


/**
 * Distribue les graines d'un trou pour une couleur selon les règles du jeu.
 * Retourne l'indice du dernier trou où une graine a été déposée.
 */
int distribuerGraines(Jeu* jeu, int trou, int couleur) {
    DEBUG_PRINT("[DEBUG] Début de distribuerGraines(trou = %d, couleur = %d)\n", trou, couleur);

    int grainesCouleur = 0;
    int grainesTransparente = 0;

    // R ou TR => step = 1, B ou TB => step = 2
    int step = (couleur % 2) + 1; 

    DEBUG_PRINT("[DEBUG] Step pour la distribution = %d\n", step);

    // On prend les graines de la couleur principale (R ou B)
    grainesCouleur = prendreGraines(jeu, trou, couleur % 2);

    DEBUG_PRINT("[DEBUG] Graines de couleur %d prises : %d\n", couleur % 2, grainesCouleur);

    // Si on prend les transparentes (TR ou TB)
    if (couleur >= 2) {
        grainesTransparente = prendreGraines(jeu, trou, couleur);
        DEBUG_PRINT("[DEBUG] Graines transparentes prises : %d\n", grainesTransparente);
    }

    int trouActuel = (trou + 1) % 16;
    
    DEBUG_PRINT("[DEBUG] Distribution commence au trou %d\n", trouActuel);

    // Distribution des graines transparentes
    while (grainesTransparente > 0) {
        if (trouActuel == trou) {
            // Sauter le trou d'origine
            trouActuel = (trouActuel + step) % 16;
        }

        ajouterGraine(jeu, trouActuel, 2);
        
        DEBUG_PRINT("[DEBUG] Ajout d'une graine transparente au trou %d (restantes = %d)\n", trouActuel, grainesTransparente - 1);
        
        grainesTransparente--;
        trouActuel = (trouActuel + step) % 16;
    }

    // Distribution des graines de couleur
    while (grainesCouleur > 0) {
        if (trouActuel == trou) {
            // Sauter le trou d'origine
            trouActuel = (trouActuel + step) % 16;
        }

        ajouterGraine(jeu, trouActuel, couleur % 2);

        DEBUG_PRINT("[DEBUG] Ajout d'une graine de couleur %d au trou %d (restantes = %d)\n", couleur % 2, trouActuel, grainesCouleur - 1);
        
        grainesCouleur--;
        trouActuel = (trouActuel + step) % 16;
    }

    // Retour au dernier trou distribué
    trouActuel = (trouActuel - step + 16) % 16;

    DEBUG_PRINT("[DEBUG] Dernier trou distribué : %d\n", trouActuel);
    DEBUG_PRINT("[DEBUG] Fin de distribuerGraines()\n\n");

    return trouActuel;
}


/* Vide un trou */
void viderTrou(Jeu* jeu, int trou) {
    jeu->rouge[trou] = 0;
    jeu->bleu[trou] = 0;
    jeu->transparent[trou] = 0;
}


/* Retourne le nombre de graines total de graines (R+B+T) dans un trou */
int recupererNbGrainesTotal(Jeu* jeu, int trou) {
    return jeu->rouge[trou] + jeu->bleu[trou] + jeu->transparent[trou];
}


/**
 * Vérifie si le camp d'un joueur est affamé (aucun coup possible)
 * Retourne true s'il n'y a AUCUNE graine sur ses trous.
 */
bool estAffame(Jeu* jeu, int adversaire) {
    int trou = adversaire;
    while (trou < 16) {
        if (recupererNbGrainesTotal(jeu, trou) > 0) {
            return false; 
        }
        trou += 2;
    }
    return true; 
}


/**
 * On commence au dernier trou où on a distribué.
 * On capture tant que les trous successifs contiennent 2 ou 3 graines,
 * en remontant trou par trou (sens inverse).
 */
void capturerGraines(Jeu* jeu, int trou) {
    DEBUG_PRINT("[DEBUG] Début de capturerGraines(trou = %d)\n", trou);

    int trouActuel = trou;
    int nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);

    DEBUG_PRINT("[DEBUG] Nombre de graines initial dans le trou %d : %d\n", trouActuel, nbGrainesTotal);

    // Capture en remontant tant que les trous contiennent 2 ou 3 graines 
    while (nbGrainesTotal == 2 || nbGrainesTotal == 3) {
        DEBUG_PRINT("[DEBUG] → Trou %d contient %d graines → capture !\n", trouActuel, nbGrainesTotal);

        // Vider le trou et mettre à jour le score
        viderTrou(jeu, trouActuel);
        jeu->score[jeu->joueurActuel] += nbGrainesTotal;

        DEBUG_PRINT("[DEBUG]    - Trou %d vidé.\n", trouActuel);
        DEBUG_PRINT("[DEBUG]    - Score joueur %d = %d\n", jeu->joueurActuel, jeu->score[jeu->joueurActuel]);

        // Passer au trou précédent (boucle circulaire)
        trouActuel = (trouActuel - 1 + 16) % 16;
        nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);

        DEBUG_PRINT("[DEBUG]    - Passage au trou %d (nbGraines = %d)\n", trouActuel, nbGrainesTotal);
    }

    // Si l'adversaire est affamé, le joueur courant prend toutes les graines restantes
    if (estAffame(jeu, donnerAdversaire(jeu))) {
        int totalPris = 96 - (jeu->score[0] + jeu->score[1]);
        jeu->score[jeu->joueurActuel] += totalPris;

        DEBUG_PRINT("[DEBUG] Adversaire affamé → capture de toutes les graines restantes (%d)\n", totalPris);
    }


    DEBUG_PRINT("[DEBUG] Fin de la boucle de capture. Condition non remplie (trou %d : %d graines)\n", trouActuel, nbGrainesTotal);
    DEBUG_PRINT("[DEBUG] Fin de capturerGraines()\n\n");
}

/* ---------------------------------------------
 *    LECTURE / ÉCRITURE D'UN COUP
 * ---------------------------------------------
 */


Coup* lireCoup(Jeu* jeu) {
    char buffer[6];

    PRINT_PRINT("joueur %d \n", jeu->joueurActuel+1);
    PRINT_PRINT("Entrez le coup : ");
    fgets(buffer, 5, stdin);

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    DEBUG_PRINT("[DEBUG] Coup lu : '%s'\n", buffer);

    bool estTransparent = strchr(buffer, 'T') != NULL || strchr(buffer, 't') != NULL;
    DEBUG_PRINT("[DEBUG] Coup transparent ? %s\n", estTransparent ? "oui" : "non");

    char couleurChar = toupper(buffer[strlen(buffer) - 1]);
    DEBUG_PRINT("[DEBUG] Caractère couleur lu : '%c'\n", couleurChar);

    int trou;
    char tmp[3];

    if (estTransparent) {
        strncpy(tmp, buffer, strlen(buffer) - 2);
        tmp[strlen(buffer) - 2] = '\0';
        DEBUG_PRINT("[DEBUG] Extraction coup transparent, tmp = '%s'\n", tmp);
    } else {
        strncpy(tmp, buffer, strlen(buffer) - 1);
        tmp[strlen(buffer) - 1] = '\0';
        DEBUG_PRINT("[DEBUG] Extraction coup normal, tmp = '%s'\n", tmp);
    }

    trou = atoi(tmp) - 1;
    DEBUG_PRINT("[DEBUG] Trou choisi : %d (index 0-based)\n", trou);

    int couleur = (estTransparent * 2) + ((couleurChar == 'R') ? 0 : 1);
    DEBUG_PRINT("[DEBUG] Couleur calculée : %d\n", couleur);
    DEBUG_PRINT("         -> 0 = Rouge, 1 = Bleu, 2 = Transparent Rouge, 3 = Transparent Bleu\n");

    return creerCoup(trou, couleur);
}


char* sortirCoup(Coup* coup) {
    char* couleurChar;
    if (coup->couleur == 0) {
        couleurChar = "R";
    } else if (coup->couleur == 1) {
        couleurChar = "B";
    } else if (coup->couleur == 2) {
        couleurChar = "TR";
    } else {
        couleurChar = "TB"; 
    }

    char* returnCoup = malloc(8);
    sprintf(returnCoup, "%d%s", coup->trou + 1, couleurChar);

    return returnCoup;
}


int donnerGagnant(Jeu* jeu) {
    if (jeu->score[0] > jeu->score[1]) {
        return 1;
    } else if (jeu->score[1] > jeu->score[0]) {
        return 2;
    } else {
        return 0;
    }
}


bool estFinPartie(Jeu* jeu) {
    if (jeu->nbCoups >= 400) {
        return true;
    }

    if (jeu->score[0] >= 49) {
        return true;
    } else if (jeu->score[1] >= 49) {
        return true;
    } else if (96 - (jeu->score[0] + jeu->score[1]) < 10) {
        return true;
    } else {
        return false;
    }
}


void afficherJeu(Jeu* jeu) {
    PRINT_PRINT("=== ÉTAT DU JEU ===\n\n");

    for (int i = 0; i < 16; i++) {
        PRINT_PRINT("trou %2d: R: %2d | B: %2d | T: %2d \n", i+1, jeu->rouge[i], jeu->bleu[i], jeu->transparent[i]);
    }
    PRINT_PRINT("\n\n");

    PRINT_PRINT("Score : 1 = %2d | 2 = %2d\n", jeu->score[0], jeu->score[1]);
    if (estFinPartie(jeu)) {
        PRINT_PRINT("=== PARTIE TERMINÉE ===\n");
        int gagnant = donnerGagnant(jeu);
        if (gagnant == 0) {
            PRINT_PRINT("Match nul !\n");
        } else {
            PRINT_PRINT("Le joueur %d a gagné la partie !\n", gagnant);
        }
    }
    else {
        PRINT_PRINT("Joueur actuel : %s\n", jeu->joueurActuel ? "2" : "1");
    }

    PRINT_PRINT("====================\n");
}


Jeu* copierJeu(const Jeu* src) {
    Jeu* dst = (Jeu*)malloc(sizeof(Jeu));

    for (int i = 0; i < 16; i++) {
        dst->rouge[i] = src->rouge[i];
        dst->bleu[i] = src->bleu[i];
        dst->transparent[i] = src->transparent[i];
    }

    dst->score[0] = src->score[0];
    dst->score[1] = src->score[1];

    dst->joueurMachine = src->joueurMachine;
    dst->joueurActuel = src->joueurActuel;
    dst->t = src->t;
    dst->nbCoups = src->nbCoups;

    return dst;
}


void jouerCoup(Jeu* jeu, Coup* coup) {
    int trou = coup->trou;
    int couleur = coup->couleur;

    DEBUG_PRINT("Joueur %d joue: trou=%d, couleur=%d\n", jeu->joueurActuel, trou, couleur);

    int dernier_trou = distribuerGraines(jeu, trou, couleur);
    DEBUG_PRINT("Dernier trou après distribution: %d\n", dernier_trou);

    capturerGraines(jeu, dernier_trou);
    DEBUG_PRINT("État du plateau après capture: joueur %d\n", jeu->joueurActuel);
    //afficherJeu(jeu); // optionnel pour un affichage complet à chaque coup

    jeu->joueurActuel = donnerAdversaire(jeu);
    jeu->nbCoups++;

    DEBUG_PRINT("Changement de joueur: joueurActuel=%d, nbCoups=%d\n", jeu->joueurActuel, jeu->nbCoups);
}

int genererCoupsEnfants(Jeu* jeu, Coup** coupsEnfants) {
    int nbCoupsEnfants = 0; // Nombre de coups générés

    for (int trou = jeu->joueurActuel; trou < 16; trou+= 2) { // += 2 car on saute les trous adverse
        if (jeu->rouge[trou] > 0) {
            coupsEnfants[nbCoupsEnfants]->trou = trou;
            coupsEnfants[nbCoupsEnfants]->couleur = 0;
            nbCoupsEnfants++;
        }

        if (jeu->bleu[trou] > 0) {
            coupsEnfants[nbCoupsEnfants]->trou = trou;
            coupsEnfants[nbCoupsEnfants]->couleur = 1;
            nbCoupsEnfants++;
        }

        if (jeu->transparent[trou] > 0) {
            coupsEnfants[nbCoupsEnfants]->trou = trou;
            coupsEnfants[nbCoupsEnfants]->couleur = 2;
            nbCoupsEnfants++;

            coupsEnfants[nbCoupsEnfants]->trou = trou;
            coupsEnfants[nbCoupsEnfants]->couleur = 3;
            nbCoupsEnfants++;
        }
    }

    return nbCoupsEnfants; // Retourne le nombre de coups enfant pour connaitre la largeur de l'arbre
}


Coup** creerCoupsEnfants() {
    Coup** coupsEnfants = (Coup**)malloc(32 * sizeof(Coup*)); // 8 trous * 4 couleurs possibles = 32 coups max
    
    for (int i = 0; i < 32; i++) {
        coupsEnfants[i] = creerCoup(0, 0); // Initialisation des coups
    }

    return coupsEnfants;
}


void libererCoupsEnfants(Coup** coupsEnfants) {
    for (int i = 0; i < 32; i++) {
        libererCoup(coupsEnfants[i]);
    }

    free(coupsEnfants);
}


// Fonction pour générer un coup aléatoire lors du plogeon du MCTS
Coup* creerCoupAleatoire(Jeu* jeu) {
    Coup* coupAleatoire = creerCoup(-1, -1);

    if (estFinPartie(jeu)) {
        PRINT_PRINT("Fin de partie");
        return coupAleatoire;
    }

    Coup** coupsPossibles = creerCoupsEnfants();
    int nbCoupsPossibles = genererCoupsEnfants(jeu, coupsPossibles);

    int indexCoupAleatoire = rand() % nbCoupsPossibles;

    coupAleatoire->trou = coupsPossibles[indexCoupAleatoire]->trou;
    coupAleatoire->couleur = coupsPossibles[indexCoupAleatoire]->couleur;

    libererCoupsEnfants(coupsPossibles);

    return coupAleatoire;
}