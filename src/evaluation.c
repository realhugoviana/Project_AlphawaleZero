#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

#include "jeu.h"
#include "evaluation.h"

#define DEBUG_MODE 0   // mettre 0 pour désactiver

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


double evalFinPartie(Jeu jeu) {
    double gainMax = 1000.0;
    double gainMin = -1000.0;
    double matchNul = 0.0;

    if (jeu.score[0] >= 49) {
        return gainMax;
    } else if (jeu.score[1] >= 49) {
        return gainMin;
    } else if (jeu.score[0] == 40 && jeu.score[1] == 40) {
        return matchNul; 
    } else if (96 - (jeu.score[0] + jeu.score[1]) < 10) {
        if (jeu.score[0] > jeu.score[1]) {
            return gainMax;
        } else if (jeu.score[1] > jeu.score[0]) {
            return gainMin;
        } else {
            return matchNul;
        }
    }
    
    return 0.0; // Partie non terminée
}


double maxScore(Jeu jeu) {
    return jeu.score[0] - jeu.score[1];
}

