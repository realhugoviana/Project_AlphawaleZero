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