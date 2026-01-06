#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"
#include "mcts.h"

#define IA_MODE 2 // 0: Joueur vs Joueur, 1: Joueur vs IA, 2: IA vs IA

#define PROFONDEUR_IA_1 1
#define ALGO_IA_1 alphaBetaVariable
#define EVAL_IA_1 evalMinChoix

#define PROFONDEUR_IA_2 1
#define ALGO_IA_2 alphaBetaVariable
#define EVAL_IA_2 evalMinChoix

#define JOUEUR_MACHINE 1
#define DUREE_SLEEP 0

int main() {
    srand(42);

    Jeu* jeu = initJeu(JOUEUR_MACHINE); 
    afficherJeu(jeu);

    while (!estFinPartie(jeu)) {
        Coup* coup = NULL;
        double timeSpent = 0.0;
        clock_t start, end;

        // --- Mode Joueur vs Joueur ---
        if (IA_MODE == 0) {
            coup = lireCoup(jeu);
        }

        // --- Mode Joueur vs IA ---
        else if (IA_MODE == 1) {
            if (jeu->joueurActuel == jeu->joueurMachine) { 
                // IA joue
                start = clock();
                coup = choisirMeilleurCoup(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
                end = clock();
                timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
            } else { 
                // Joueur humain
                coup = lireCoup(jeu);
            }
        }

        // --- Mode IA vs IA ---
        else if (IA_MODE == 2) {
            start = clock();
            if (jeu->joueurActuel == 0) 
                coup = choisirMeilleurCoupIteratifVariable(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
            else 
                coup = choisirMeilleurCoupMCTS(jeu, PROFONDEUR_IA_2, ALGO_IA_2, EVAL_IA_2);
            end = clock();
            timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
            sleep(DUREE_SLEEP);
        }

        jouerCoup(jeu, coup);
        afficherJeu(jeu);
        sortirCoup(coup);

        if (IA_MODE != 0) 
            printf("Temps de calcul : %.8f secondes\n", timeSpent);

        libererCoup(coup);
        printf("Coup numéro : %d\n", jeu->nbCoups);
    }

    printf("Partie terminée.\n");
    return 0;
}