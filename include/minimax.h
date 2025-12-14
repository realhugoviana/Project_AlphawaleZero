#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include "jeu.h"
#include "evaluation.h"
#include <time.h>

typedef struct {
    clock_t debut;     
    double limiteTemps; // limite en secondes
    bool estFinTemps;   // passé à true si on dépasse
} Temps;

typedef struct Jeu  Jeu;
typedef struct Coup Coup;


double minimax(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*));
double alphaBeta(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*));
double alphaBetaVariable(Jeu* jeu, int profondeur, double alpha, double beta, bool maximisant, double (*evaluation)(Jeu*));

Coup* choisirMeilleurCoup(Jeu* jeu, int profondeurMax, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*));
Coup* choisirMeilleurCoupIteratif(Jeu* jeu, int profondeurMax, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*));
Coup* choisirMeilleurCoupIteratifVariable(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*));

#endif // MINIMAX_H