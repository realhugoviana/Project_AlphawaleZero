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


#define PRINT_MODE 0

# if PRINT_MODE
    #define PRINT_PRINT(...) printf(__VA_ARGS__)
# else
    #define PRINT_PRINT(...)
#endif


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

    PRINT_PRINT("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

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

    PRINT_PRINT("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

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

    PRINT_PRINT("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

    double limiteTempsDouble = (double)limiteTempsInt - 0.5; 

    Temps t;
    jeu->t.debut = clock();
    jeu->t.limiteTemps = limiteTempsDouble; 
    jeu->t.estFinTemps = false;

    for (int profondeur = 1; profondeur <= 50; profondeur++) {
        PRINT_PRINT("Profondeur actuelle : %d\n", profondeur);

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


Coup* choisirMeilleurCoupAleatoireIteratifVariable(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    int indexMeilleurCoup = -1;
    double meilleurScore = -10000;
    const int signeRacine = (jeu->joueurActuel == 0) ? +1 : -1;

    Coup** coupsEnfants = creerCoupsEnfants();
    int nbCoupsEnfants = genererCoupsEnfants(jeu, coupsEnfants);

    PRINT_PRINT("Nombre de coups enfants générés : %d\n", nbCoupsEnfants);

    double limiteTempsDouble = (double)limiteTempsInt - 0.1; 

    Temps t;
    jeu->t.debut = clock();
    jeu->t.limiteTemps = limiteTempsDouble; 
    jeu->t.estFinTemps = false;

    for (int profondeur = 1; profondeur <= 50; profondeur++) {
        PRINT_PRINT("Profondeur actuelle : %d\n", profondeur);

        double scores[nbCoupsEnfants];
        int nbEvalues = 0;

        for (int i = 0; i < nbCoupsEnfants; i++) {
            verifierFinDuTemps(&jeu->t);
            if (jeu->t.estFinTemps) break;

            Jeu* jeuCopie = copierJeu(jeu);
            jouerCoup(jeuCopie, coupsEnfants[i]);

            bool estMax = (jeuCopie->joueurActuel == 0);

            double score = signeRacine * minimax(jeuCopie, profondeur - 1, -10000, 10000, estMax, evaluation);
            
            scores[i] = score;
            nbEvalues++;

            libererJeu(jeuCopie);

            if (jeu->t.estFinTemps) break;

            if (score > meilleurScore) {
                meilleurScore = score;
            }

        }

        verifierFinDuTemps(&jeu->t);
        if (jeu->t.estFinTemps) break;

        // Seuil à 90% (négatif et positif)
        double ratio = 0.9;
        double seuil = (meilleurScore >= 0) ? (ratio * meilleurScore) : (meilleurScore / ratio);

        // Liste des coups candidats
        int candidatsIndices[nbCoupsEnfants];
        int nbCandidats = 0;

        for (int i = 0; i < nbEvalues; i++) {
            if (scores[i] >= seuil) {
                candidatsIndices[nbCandidats] = i;
                nbCandidats++;
            }
        }

        // Choix aléatoire parmi les candidats
        int indiceAleatoire = rand() % nbCandidats;
        indexMeilleurCoup = candidatsIndices[indiceAleatoire];

        // trie les coups
        Coup* temp = coupsEnfants[0];
        coupsEnfants[0] = coupsEnfants[indexMeilleurCoup];
        coupsEnfants[indexMeilleurCoup] = temp;

        // reset
        indexMeilleurCoup = -1;
        meilleurScore = -10000;
    }

    Coup* meilleurCoup = creerCoup(coupsEnfants[0]->trou, coupsEnfants[0]->couleur);

    libererCoupsEnfants(coupsEnfants);

    return meilleurCoup;
}