#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"


#define DEBUG_MODE 0   // mettre 0 pour désactiver

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


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


double minimax(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*)) {
    if (profondeur == 0) {
        return evaluation(jeu);
    } 

    if (estFinPartie(jeu)) {
        return evalFinPartie(jeu);
    }

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    if (maximisant) {
        double maxEval = -10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = minimax(jeuCopie, profondeur - 1, -10000, 10000, false, evaluation);

            if (eval > maxEval) {
                maxEval = eval;
            }

            libererJeu(jeuCopie);
        }

        libererCoupsEnfants(coupsEnfants);

        return maxEval;

    } else {
        double minEval = 10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = minimax(jeuCopie, profondeur - 1, -10000, 10000, true, evaluation);

            if (eval < minEval) {
                minEval = eval;
            }

            libererJeu(jeuCopie);
        }

        libererCoupsEnfants(coupsEnfants);

        return minEval;
    }

}


double alphaBeta(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*)) {
    if (profondeur == 0) {
        return evaluation(jeu);
    } 

    if (estFinPartie(jeu)) {
        return evalFinPartie(jeu);
    }

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    if (maximisant) {
        double maxEval = -10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBeta(jeuCopie, profondeur - 1, alpha, beta, false, evaluation);

            if (eval > maxEval) {
                maxEval = eval;
            }
            if (eval > alpha) {
                alpha = eval;
            }
            libererJeu(jeuCopie);

            if (beta <= alpha) {
                break; 
            }
        }

        libererCoupsEnfants(coupsEnfants);

        return maxEval;

    } else {
        double minEval = 10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBeta(jeuCopie, profondeur - 1, alpha, beta, true, evaluation);

            if (eval < minEval) {
                minEval = eval;
            }
            if (eval < beta) {
                beta = eval;
            }
            libererJeu(jeuCopie);

            if (beta <= alpha) {
                break; 
            }
        }

        libererCoupsEnfants(coupsEnfants);

        return minEval;
    }

}


double alphaBetaVariable(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*)) {
    if (profondeur <= 0) {
        return evaluation(jeu);
    } 

    if (estFinPartie(jeu)) {
        return evalFinPartie(jeu);
    }

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    if (nbCoupsEnfants > 28) {
        profondeur -= 5; 
    } else if (nbCoupsEnfants > 20 ) {
        profondeur -= 4; 
    } else if (nbCoupsEnfants > 12 ) {
        profondeur -= 3; 
    } else if (nbCoupsEnfants > 6 ) {
        profondeur -= 2; 
    } else if (nbCoupsEnfants > 3 ) {
        profondeur -= 1; 
    }   

    if (maximisant) {
        double maxEval = -10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBetaVariable(jeuCopie, profondeur, alpha, beta, false, evaluation);

            if (eval > maxEval) {
                maxEval = eval;
            }
            if (eval > alpha) {
                alpha = eval;
            }
            libererJeu(jeuCopie);

            if (beta <= alpha) {
                break; 
            }
        }

        libererCoupsEnfants(coupsEnfants);

        return maxEval;

    } else {
        double minEval = 10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBetaVariable(jeuCopie, profondeur, alpha, beta, true, evaluation);

            if (eval < minEval) {
                minEval = eval;
            }
            if (eval < beta) {
                beta = eval;
            }
            libererJeu(jeuCopie);

            if (beta <= alpha) {
                break; 
            }
        }

        libererCoupsEnfants(coupsEnfants);

        return minEval;
    }

}

Coup* choisirMeilleurCoup(Jeu* jeu, int profondeur, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    Coup* meilleurCoup = creerCoup(-1, -1); 
    double meilleurScore = -10000;
    const int signeRacine = (jeu->joueurActuel == 0) ? +1 : -1;

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    printf("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

    for (int i = 0; i < nbCoupsEnfants; i++) {

        Jeu* jeuCopie = copierJeu(jeu);
        jouerCoup(jeuCopie, coupsEnfants[i]);

        bool estMax = (jeuCopie->joueurActuel == 0);

        double score = signeRacine * minimax(jeuCopie, profondeur - 1, -10000, 10000, estMax, evaluation);

        if (score > meilleurScore) {
            meilleurScore = score;
            meilleurCoup->trou = coupsEnfants[i]->trou;
            meilleurCoup->couleur = coupsEnfants[i]->couleur;
        }

        libererJeu(jeuCopie);
    }

    libererCoupsEnfants(coupsEnfants);

    return meilleurCoup;
}
