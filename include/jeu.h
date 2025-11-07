#ifndef JEU_H
#define JEU_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

typedef struct {
    int rouge[16];
    int bleu[16];
    int transparent[16];
    int score[2];
    bool joueurMachine;
    bool joueurActuel;
    char coup[6];
} Jeu;

Jeu* initJeu(bool joueurMachine);

void ajouterGraine(Jeu* jeu, int trou, int couleur);

void retirerGraine(Jeu* jeu, int trou, int couleur);

int prendreGraines(Jeu* jeu, int trou, int couleur);

int distribuerGraines(Jeu* jeu, int trou, int couleur);

void viderTrou(Jeu* jeu, int trou);
 
int recupererNbGrainesTotal(Jeu* jeu, int trou);

void capturerGraines(Jeu* jeu, int trou);

void lireCoup(Jeu* jeu);

void jouerTour(Jeu* jeu);

void afficherJeu(Jeu* jeu);

void jouerPartie();

#endif // JEU_H