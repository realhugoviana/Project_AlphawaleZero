#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <math.h>

#include "jeu.h"
#include "evaluation.h"

#define DEBUG_MODE 0   // mettre 0 pour désactiver

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


double evalFinPartie(Jeu* jeu) {
    double gainMax = 1000.0;
    double gainMin = -1000.0;
    double matchNul = 0.0;

    if (jeu->score[0] >= 49) {
        return gainMax;
    } else if (jeu->score[1] >= 49) {
        return gainMin; 
    } else if (96 - (jeu->score[0] + jeu->score[1]) < 10 || jeu->nbCoups >= 400) {
        if (jeu->score[0] > jeu->score[1]) {
            return gainMax;
        } else if (jeu->score[1] > jeu->score[0]) {
            return gainMin;
        } else {
            return matchNul;
        }
    }

    return 0.0; // Partie non terminée
}


double maxScore(Jeu* jeu) {
    return jeu->score[0] - jeu->score[1];
}


double evalMinChoix(Jeu* jeu) {
    int mobilite = 0;

    for (int trou = !jeu->joueurActuel; trou < 16; trou+=2) { 
        if (jeu->rouge[trou] > 0) {
            mobilite++;
        }

        if (jeu->bleu[trou] > 0) {
            mobilite++;
        }

        if (jeu->transparent[trou] > 0) {
            mobilite+=2;
        }
    }

    int score = jeu->score[0] - jeu->score[1];

    int signe = (jeu->joueurActuel == 0) ? -1 : 1;

    double coefS = 1;
    double coefM = 0.5;

    return coefS * score + signe * coefM * mobilite;
}


double evalPartieCourte(Jeu* jeu) {
    int mobilite = 0;

    for (int trou = !jeu->joueurActuel; trou < 16; trou += 2) {
        if (jeu->rouge[trou] > 0) mobilite++;
        if (jeu->bleu[trou] > 0) mobilite++;
        if (jeu->transparent[trou] > 0) mobilite += 2;
    }

    int score = jeu->score[0] - jeu->score[1];
    int signe = (jeu->joueurActuel == 0) ? -1 : 1;

    double coefS = 1.0;
    double coefM = 1.0;

    // pénalisation forte du jeu long 
    const double CMAX = 400.0;
    double p = 1.0 - (jeu->nbCoups / CMAX);
    if (p < 0.0) p = 0.0;        
    const int k = 6;               
    double penaliteJeuLong = pow(p, k);

    return (coefS * score + signe * coefM * mobilite) * penaliteJeuLong;
}
