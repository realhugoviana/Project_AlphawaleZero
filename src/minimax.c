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


Coup minimax(Jeu* jeu, int profondeur, int (*evaluation)(Jeu)) {
    // Implémentation de l'algorithme Minimax
    // Cette fonction doit retourner le meilleur coup pour le joueur actuel
    // en fonction de la profondeur spécifiée et de la fonction d'évaluation donnée.

    int meilleurEvaluation = -10000;
    if (profondeur == 0 || estFinPartie(jeu)) {
        jouerCoup(jeu);
        meilleurEvaluation = evaluation(jeu);
    }
}


int genererCoupsEnfants(Jeu* jeu, Coup** coupsEnfants) {
    int nbCoupsEnfants = 0; // Nombre de coups générés
    int trou = jeu->joueurActuel; // trou est égal à 0 si c'est au tour du joueur 1 et 1 sinon

    for (int trou; trou < 16; trou+= 2) { // += 2 car on saute les trous adverse
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