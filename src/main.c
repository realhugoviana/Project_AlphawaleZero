#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"


int main() {
    Jeu* jeu = initJeu(0);

    afficherJeu(jeu);
    while (!estFinPartie(jeu)) {
        Coup* coup;
        double timeSpent = 0.0;
        if (jeu->joueurActuel == jeu->joueurMachine) {
            clock_t start = clock();
            coup = choisirMeilleurCoup(jeu, 5, minimax, maxScore);
            clock_t end = clock();
            timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
        } else {
            coup = lireCoup(jeu);
        }

        jouerCoup(jeu, coup);

        afficherJeu(jeu);
        printf("Coup joué : Trou %d, Couleur %d\n", coup->trou + 1, coup->couleur);
        printf("Temps de calcul : %.2f secondes\n", timeSpent);

        libererCoup(coup);
    }

    printf("Partie terminée.\n");
}