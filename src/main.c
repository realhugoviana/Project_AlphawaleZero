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
#define ALGO_IA_1 alphaBetaVariableAleatoire
#define EVAL_IA_1 evalMinChoix

#define PROFONDEUR_IA_2 1
#define ALGO_IA_2 alphaBetaVariable
#define EVAL_IA_2 evalMinChoix

#define JOUEUR_MACHINE 1
#define DUREE_SLEEP 0

static int readLine(char *buf, size_t n) {
    if (!fgets(buf, (int)n, stdin)) return 0;
    size_t len = strlen(buf);
    while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) {
        buf[--len] = '\0';
    }
    return 1;
}


int main(int argc, char** argv) {
    srand(42);

    int idMachine = 0; // par défaut JoueurA

    if (argc >= 2) {
        if (strcmp(argv[1], "JoueurB") == 0 || strcmp(argv[1], "B") == 0) {
            idMachine = 1;
        } else if (strcmp(argv[1], "JoueurA") == 0 || strcmp(argv[1], "A") == 0) {
            idMachine = 0;
        }
    }

    Jeu* jeu = initJeu(idMachine); 
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
                coup = choisirMeilleurCoupAleatoireIteratifVariable(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
            else 
                coup = choisirMeilleurCoupIteratifVariable(jeu, PROFONDEUR_IA_2, ALGO_IA_2, EVAL_IA_2);
                
            // print le coup joué format juste "1TR"
            end = clock();
            timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
            sleep(DUREE_SLEEP);
        }

        // Mode évaluation uniquement
        else if (IA_MODE == 3) {
            char etat[256];

            while (readLine(etat, sizeof(etat))) {

                if (strcmp(etat, "END") == 0) break;

                if (strcmp(etat, "START") != 0) {
                    if (strncmp(etat, "RESULT", 6) == 0) break;

                    Coup* coupAdv = coupDepuisString(etat);

                    jouerCoup(jeu, coupAdv);
                    libererCoup(coupAdv);
                }

                // Notre algo IA
                Coup* coup = choisirMeilleurCoupIteratifVariable(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);

                const char* sCoup = sortirCoup(coup);

                jouerCoup(jeu, coup);

                if (jeu->nbCoups >= 400) {
                    printf("RESULT LIMIT %d %d\n", jeu->score[0], jeu->score[1]);
                } else if (estFinPartie(jeu)) {
                    printf("RESULT %s %d %d\n", sCoup, jeu->score[0], jeu->score[1]);
                } else {
                    printf("%s\n", sCoup);
                }
                fflush(stdout);

                libererCoup(coup);
            }
            return 0;
        }

        jouerCoup(jeu, coup);
        
        if (IA_MODE < 3) {
            afficherJeu(jeu);

            printf("Temps de calcul : %.8f secondes\n", timeSpent);
            printf("Coup numéro : %d\n", jeu->nbCoups);
            printf("Coup joué : %s\n", sortirCoup(coup));
            printf("\n");
        }

        libererCoup(coup);
       
    }

    return 0;
}