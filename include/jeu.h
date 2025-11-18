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

Jeu* copierJeu(const Jeu* src);

void libererJeu(Jeu* jeu);

Coup* creerCoup(int trou, int couleur);

void libererCoup(Coup* coup);
 
int recupererNbGrainesTotal(Jeu* jeu, int trou);

Coup* lireCoup(Jeu* jeu);

void jouerCoup(Jeu* jeu, Coup* coup);

bool estFinPartie(Jeu* jeu);

void afficherJeu(Jeu* jeu);

void sortirCoup(Coup* coup);

#endif // JEU_H