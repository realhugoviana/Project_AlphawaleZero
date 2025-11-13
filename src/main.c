#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"

int main() {
    Jeu* jeu = initJeu(0);

    afficherJeu(jeu);
    while (!estFinPartie(jeu)) {
        Coup* coup;
        if (jeu->joueurActuel == jeu->joueurMachine) {
            coup = choisirMeilleurCoup(jeu, 2, minimax, maxScore);
        } else {
            coup = lireCoup(jeu);
        }
        jouerCoup(jeu, coup);

        afficherJeu(jeu);
    }

    printf("Partie terminée.\n");
}