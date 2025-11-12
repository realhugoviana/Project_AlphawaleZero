#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include "jeu.h"
#include "evaluation.h"

typedef struct {
    Jeu etat;
    int evaluation;
    int profondeur;
    int nombreEnfants;
    Noeud** enfant;
    Coup coup;
} Noeud;

Coup minimax(Jeu* jeu, int profondeur, int (*evaluation)(Jeu*));

#endif // MINIMAX_H