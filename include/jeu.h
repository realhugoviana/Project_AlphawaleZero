#ifndef JEU_H
#define JEU_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

typedef struct {
    int trou;
    int couleur;
} Coup;

typedef struct {
    int rouge[16];
    int bleu[16];
    int transparent[16];
    int score[2];
    bool joueurMachine;
    bool joueurActuel;
} Jeu;

Jeu* initJeu(bool joueurMachine);

void copierJeu(const Jeu* src, Jeu* dst);

void libererJeu(Jeu* jeu);

void libererCoup(Coup* coup);
 
int recupererNbGrainesTotal(Jeu* jeu, int trou);

void lireCoup(Jeu* jeu);

void jouerTour(Jeu* jeu);

bool estFinPartie(Jeu* jeu);

void afficherJeu(Jeu* jeu);

#endif // JEU_H