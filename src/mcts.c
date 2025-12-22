#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"
#include "mcts.h"


#define DEBUG_MODE 0  // mettre 0 pour désactiver

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


NoeudMCTS* creerNoeudMCTS(Jeu* jeu, int trou, int couleur, int n, int score, NoeudMCTS* parent) {
    NoeudMCTS* noeud = (NoeudMCTS*)malloc(sizeof(NoeudMCTS));
    noeud->jeu = copierJeu(jeu);
    noeud->coup = creerCoup(trou, couleur);
    noeud->n = n;
    noeud->scoreTotal = score;
    noeud->parent = parent;
    noeud->enfants = NULL;
    noeud->nbEnfants = 0;

    if (trou != -1 && couleur != -1)
        jouerCoup(noeud->jeu, noeud->coup);

    return noeud;
}


// Fonction pour générer les noeuds enfants pour le MCTS
void genererNoeudsMCTSEnfants(NoeudMCTS* parent) {
    parent->enfants = (NoeudMCTS**)malloc(32 * sizeof(NoeudMCTS*)); // 8 trous * 4 couleurs possibles = 32 coups max

    for (int trou = parent->jeu->joueurActuel; trou < 16; trou+= 2) {
        if (parent->jeu->rouge[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 0, 0, 0, parent);
            parent->nbEnfants++;
        }  
        if (parent->jeu->bleu[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 1, 0, 0, parent);
            parent->nbEnfants++;
        }
        if (parent->jeu->transparent[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 2, 0, 0, parent);
            parent->nbEnfants++;

            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 3, 0, 0, parent);
            parent->nbEnfants++;
        }
    }

    for (int i = parent->nbEnfants - 1; i > 0; i--) {
        int j = rand() % (i+1);
        NoeudMCTS* temp = parent->enfants[i];
        parent->enfants[i] = parent->enfants[j];
        parent->enfants[j] = temp;
    }
}


void libererNoeudMCTS(NoeudMCTS* noeud) {
    if (noeud == NULL) return;

    for (int i = 0; i < noeud->nbEnfants; i++) {
        libererNoeudMCTS(noeud->enfants[i]);
    }
    free(noeud->enfants);

    libererJeu(noeud->jeu);

    libererCoup(noeud->coup);

    free(noeud);
}

int evalMCTS(Jeu* jeu, bool joueurRacine) {
    int victoire = 0;

    if (jeu->score[joueurRacine] > jeu->score[!joueurRacine]) {
        victoire = 1;
    }
    else if (jeu->score[!joueurRacine] > jeu->score[joueurRacine]) {
        victoire = -1;
    }

    return victoire;
}


// Fonction de plongeon pour le MCTS, génère un coup aléatoire jusqu'à une certaine profondeur et évalue la position
double plongeon(Jeu* jeu, bool joueurRacine) {
    Jeu* jeuCopie = copierJeu(jeu);
    int score;

    while (!estFinPartie(jeuCopie)) {

        Coup* coupAleatoire = creerCoupAleatoire(jeuCopie);

        DEBUG_PRINT("coup aléatoire (trou=%d, couleur=%d), fin de partie : %d\n", coupAleatoire->trou, coupAleatoire->couleur, estFinPartie(jeuCopie));
        jouerCoup(jeuCopie, coupAleatoire);
        libererCoup(coupAleatoire);
    }
    
    score = evalMCTS(jeuCopie, joueurRacine);

    DEBUG_PRINT("Score évalué après plongeon: %.2f\n", score);

    libererJeu(jeuCopie);

    return score;
}


double ucb1(NoeudMCTS* noeud, int N) {
    if (noeud->n == 0) {
        return DBL_MAX; // Priorité maximale pour les noeuds non explorés
    }

    double moyenneScore = (double)noeud->scoreTotal / (double)noeud->n;
    double exploration = 1.414 * sqrt(log(N) / noeud->n);

    return moyenneScore + exploration;
}


int indiceMeilleurUcb1(NoeudMCTS* noeud) {
    if (noeud->nbEnfants == 0) 
        return -1;

    double meilleurUcb1 = -DBL_MAX;
    int indiceMeilleurNoeud = -1;

    for (int i = 0; i < noeud->nbEnfants; i++) {
        NoeudMCTS* temp = noeud->enfants[i];

        double ucb1Temp = ucb1(temp, noeud->n);

        if (ucb1Temp > meilleurUcb1) {
            meilleurUcb1 = ucb1Temp;
            indiceMeilleurNoeud = i;
        }
    }

    return indiceMeilleurNoeud;
}


void retroPropagation(NoeudMCTS* noeud, double score) {
    NoeudMCTS* noeudActuel = noeud;

    while (noeudActuel != NULL) {
        noeudActuel->n++;
        noeudActuel->scoreTotal += score;

        noeudActuel = noeudActuel->parent;
    }

    return;
}


void parcoursMCTS(NoeudMCTS* racine, bool joueurRacine) {
    int indiceMeilleurEnfant;
    NoeudMCTS* noeudActuel = racine;
    int score;

    while (noeudActuel->nbEnfants > 0) {
        indiceMeilleurEnfant = indiceMeilleurUcb1(noeudActuel);

        noeudActuel = noeudActuel->enfants[indiceMeilleurEnfant];
    }

    if (!estFinPartie(noeudActuel->jeu)) {
        genererNoeudsMCTSEnfants(noeudActuel);

        noeudActuel = noeudActuel->enfants[0];
    }

    score = plongeon(noeudActuel->jeu, joueurRacine);

    retroPropagation(noeudActuel, score);


    return;
}


Coup* choisirMeilleurCoupMCTS(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    NoeudMCTS* racine = creerNoeudMCTS(jeu, -1, -1, 0, 0, NULL);
    genererNoeudsMCTSEnfants(racine);

    int meilleurN = -1;
    NoeudMCTS* meilleurNoeud = racine->enfants[0];

    double limiteTempsDouble = (double)limiteTempsInt - 0.01; 

    jeu->t.debut = clock();
    jeu->t.limiteTemps = limiteTempsDouble; 
    jeu->t.estFinTemps = false;

    int counter = 0;
    NoeudMCTS* temp;
    while(!jeu->t.estFinTemps) {
        counter++;
        DEBUG_PRINT("Parcours %d\n", counter);
        parcoursMCTS(racine, racine->jeu->joueurActuel);

        verifierFinDuTemps(&jeu->t);
        DEBUG_PRINT("temps écoulé : %.8f\n", (double)(clock() - jeu->t.debut) / CLOCKS_PER_SEC);
    }

    for (int i = 0; i < racine->nbEnfants; i++) {
        temp = racine->enfants[i];
        if (temp->n > meilleurN) {
            meilleurN = temp->n;
            meilleurNoeud = temp;
        }
    }

    Coup* meilleurCoup = creerCoup(meilleurNoeud->coup->trou, meilleurNoeud->coup->couleur);

    libererNoeudMCTS(racine);

    return meilleurCoup;
}