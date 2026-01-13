#ifndef EVALUATION_H
#define EVALUATION_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include "jeu.h"

typedef struct Jeu Jeu;

double maxScore(Jeu* jeu);
double evalMinChoix(Jeu* jeu);
double evalFinPartie(Jeu* jeu);
double evalPartieCourte(Jeu* jeu);

#endif // EVALUATION_H