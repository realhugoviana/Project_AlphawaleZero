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
        if (jeu->joueurActuel == jeu->joueurMachine) {
            jouerCoup(jeu);
        } else {
            lireCoup(jeu);
        }
        jouerCoups(jeu);
        //jouerTour(jeu);

        jeu->joueurActuel = donnerAdversaire(jeu);
        afficherJeu(jeu);
    }

    printf("Partie terminée.\n");
}