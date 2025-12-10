#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>

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


void verifierFinDuTemps(Temps* t) {
    double tempsEcoule = (double)(clock() - t->debut) / CLOCKS_PER_SEC;
    if (tempsEcoule >= t->limiteTemps) {
        t->estFinTemps = true;
    }
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
    if (profondeur == 0 && !jeu->t.estFinTemps) {
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
            verifierFinDuTemps(&jeu->t);
            if (jeu->t.estFinTemps) break;

            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBetaVariable(jeuCopie, profondeur - 1, alpha, beta, false, evaluation);

            if (eval > maxEval) {
                maxEval = eval;
            }
            if (eval > alpha) {
                alpha = eval;
            }

            libererJeu(jeuCopie);

            if (jeu->t.estFinTemps) break;

            if (beta <= alpha) {
                break; 
            }
        }

        libererCoupsEnfants(coupsEnfants);

        return maxEval;

    } else {
        double minEval = 10000;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            verifierFinDuTemps(&jeu->t);
            if (jeu->t.estFinTemps) break;

            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            double eval = alphaBetaVariable(jeuCopie, profondeur - 1, alpha, beta, true, evaluation);

            if (eval < minEval) {
                minEval = eval;
            }
            if (eval < beta) {
                beta = eval;
            }

            libererJeu(jeuCopie);

            if (jeu->t.estFinTemps) break;

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


Coup* choisirMeilleurCoupIteratif(Jeu* jeu, int profondeurMax, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    Coup* meilleurCoup = creerCoup(-1, -1); 
    int indexMeilleurCoup = -1;
    double meilleurScore = -10000;
    const int signeRacine = (jeu->joueurActuel == 0) ? +1 : -1;

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    printf("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

    for(int profondeur = 1; profondeur <= profondeurMax; profondeur++) {
        for (int i = 0; i < nbCoupsEnfants; i++) {

            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            bool estMax = (jeuCopie->joueurActuel == 0);

            double score = signeRacine * minimax(jeuCopie, profondeur - 1, -10000, 10000, estMax, evaluation);

            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurCoup->trou = coupsEnfants[i]->trou;
                meilleurCoup->couleur = coupsEnfants[i]->couleur;
                indexMeilleurCoup = i;
            }

            libererJeu(jeuCopie);
        }
        // trie les coups
        Coup* temp = coupsEnfants[0];
        coupsEnfants[0] = coupsEnfants[indexMeilleurCoup];
        coupsEnfants[indexMeilleurCoup] = temp;
        meilleurScore = -10000; // reset meilleur score for next depth
        indexMeilleurCoup = -1;
    }

    libererCoupsEnfants(coupsEnfants);

    return meilleurCoup;
}


Coup* choisirMeilleurCoupIteratifVariable(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    Coup* meilleurCoup = creerCoup(-1, -1); 
    int indexMeilleurCoup = -1;
    double meilleurScore = -10000;
    const int signeRacine = (jeu->joueurActuel == 0) ? +1 : -1;

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    printf("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

    double limiteTempsDouble = (double)limiteTempsInt - 0.01; 

    Temps t;
    jeu->t.debut = clock();
    jeu->t.limiteTemps = limiteTempsDouble; 
    jeu->t.estFinTemps = false;

    for (int profondeur = 1; profondeur <= 50; profondeur++) {
        printf("Profondeur actuelle : %d\n", profondeur);

        for (int i = 0; i < nbCoupsEnfants; i++) {
            verifierFinDuTemps(&jeu->t);
            if (jeu->t.estFinTemps) break;

            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            bool estMax = (jeuCopie->joueurActuel == 0);

            double score = signeRacine * minimax(jeuCopie, profondeur - 1, -10000, 10000, estMax, evaluation);
            
            libererJeu(jeuCopie);

            if (jeu->t.estFinTemps) break;

            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurCoup->trou = coupsEnfants[i]->trou;
                meilleurCoup->couleur = coupsEnfants[i]->couleur;
                indexMeilleurCoup = i;
            }

        }

        verifierFinDuTemps(&jeu->t);
        if (jeu->t.estFinTemps) break;

        // trie les coups
        Coup* temp = coupsEnfants[0];
        coupsEnfants[0] = coupsEnfants[indexMeilleurCoup];
        coupsEnfants[indexMeilleurCoup] = temp;
        meilleurScore = -10000; // reset meilleur score for next depth
        indexMeilleurCoup = -1;

    }

    meilleurCoup->trou = coupsEnfants[0]->trou;;
    meilleurCoup->couleur = coupsEnfants[0]->couleur;

    libererCoupsEnfants(coupsEnfants);

    return meilleurCoup;
}