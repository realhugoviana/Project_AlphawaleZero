#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include "jeu.h"
#include "evaluation.h"

double minimax(Jeu* jeu, int profondeur, bool maximisant, double (*evaluation)(Jeu*));

Coup choisirMeilleurCoup(Jeu* jeu, int profondeur, double (*minimax)(Jeu*, int, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*));

#endif // MINIMAX_H