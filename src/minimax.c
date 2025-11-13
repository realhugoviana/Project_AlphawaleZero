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


double minimax(Jeu* jeu, int profondeur, bool maximisant, double (*evaluation)(Jeu*)) {
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
            double eval = minimax(jeuCopie, profondeur - 1, false, evaluation);
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
            double eval = minimax(jeuCopie, profondeur - 1, true, evaluation);
            if (eval < minEval) {
                minEval = eval;
            }
            libererJeu(jeuCopie);
        }
        libererCoupsEnfants(coupsEnfants);
        return minEval;
    }

}


Coup choisirMeilleurCoup(Jeu* jeu, int profondeur, double (*minimax)(Jeu*, int, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    Coup meilleurCoup;
    double meilleurScore = -10000;

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    for (int i = 0; i < nbCoupsEnfants; i++) {
        Jeu* jeuCopie = copierJeu(jeu);
        jouerCoup(jeuCopie, coupsEnfants[i]);
        double score = minimax(jeuCopie, profondeur - 1, false, evaluation);
        if (score > meilleurScore) {
            meilleurScore = score;
            meilleurCoup = *coupsEnfants[i];
        }
        libererJeu(jeuCopie);
    }

    libererCoupsEnfants(coupsEnfants);

    return meilleurCoup;
}
