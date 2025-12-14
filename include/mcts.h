#ifndef MCTS_H
#define MCTS_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>

#include "jeu.h"
#include "evaluation.h"
#include "minimax.h"

typedef struct NoeudMCTS {
    Jeu* jeu;
    Coup* coup;
    double scoreTotal;
    int n;
    struct NoeudMCTS** enfants;
    int nbEnfants;
} NoeudMCTS;

#endif // MCTS_H