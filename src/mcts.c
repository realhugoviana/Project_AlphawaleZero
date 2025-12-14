#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"
#include "mcts.h"


#define DEBUG_MODE 0   // mettre 0 pour désactiver

#if DEBUG_MODE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


NoeudMCTS* creerNoeudMCTS(Jeu* jeu, int trou, int couleur, int n, double score) {
    NoeudMCTS* noeud = (NoeudMCTS*)malloc(sizeof(NoeudMCTS));
    noeud->jeu = copierJeu(jeu);
    noeud->coup = creerCoup(trou, couleur);
    noeud->n = n;
    noeud->scoreTotal = score;
    noeud->enfants = NULL;
    noeud->nbEnfants = 0;

    if (trou != -1 && couleur != -1)
        jouerCoup(noeud->jeu, noeud->coup);

    return noeud;
}


// Fonction pour générer les noeuds enfants pour le MCTS
void genererNoeudsMCTSEnfants(NoeudMCTS* parent) {
    parent->enfants = (NoeudMCTS**)malloc(32 * sizeof(NoeudMCTS*)); // 8 trous * 4 couleurs possibles = 32 coups max

    for (int trou = parent->jeu->joueurActuel; trou < 16; trou+= 2) { // += 2 car on saute les trous adverse
        if (parent->jeu->rouge[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 0, 0, 0.0);
            parent->nbEnfants++;
        }

        if (parent->jeu->bleu[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 1, 0, 0.0);
            parent->nbEnfants++;
        }

        if (parent->jeu->transparent[trou] > 0) {
            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 2, 0, 0.0);
            parent->nbEnfants++;

            parent->enfants[parent->nbEnfants] = creerNoeudMCTS(parent->jeu, trou, 3, 0, 0.0);
            parent->nbEnfants++;
        }
    }
}


void libererNoeudMCTS(NoeudMCTS* noeud) {
    if (noeud == NULL) return;

    for (int i = 0; i < 32; i++) {
        libererNoeudMCTS(noeud->enfants[i]);
    }
    free(noeud->enfants);

    libererJeu(noeud->jeu);

    libererCoup(noeud->coup);

    free(noeud);
}


// Fonction de plongeon pour le MCTS, génère un coup aléatoire jusqu'à une certaine profondeur et évalue la position
double plongeon(Jeu* jeu, double (*evaluation)(Jeu*), int profondeurMax) {
    Jeu* jeuCopie = copierJeu(jeu);

    for (int d = 0; d < profondeurMax; d++) {
        if (estFinPartie(jeu)) {
            break;
        }

        Coup* coupAleatoire = creerCoupAleatoire(jeu);
        jouerCoup(jeu, coupAleatoire);
        libererCoup(coupAleatoire);
    }

    double score = evaluation(jeuCopie);
    libererJeu(jeuCopie);

    return score;
}


double ucb1(NoeudMCTS* noeud, int nRacine) {
    if (noeud->n == 0) {
        return 10000.0; // Priorité maximale pour les noeuds non explorés
    }

    double moyenneScore = noeud->scoreTotal / noeud->n;
    double exploration = 1.414 * sqrt(log(nRacine) / noeud->n);

    return moyenneScore + exploration;
}

void parcoursMCTS(NoeudMCTS* noeud, double (*evaluation)(Jeu*), int profondeurMax, NoeudMCTS* racine) {
    
    if (noeud->nbEnfants == 0) {
        genererNoeudsMCTSEnfants(noeud);

        for (int i = 0; i < noeud->nbEnfants; i++) {
            NoeudMCTS* temp = noeud->enfants[i];

            temp->scoreTotal = plongeon(temp->jeu, evaluation, profondeurMax);
            temp->n = 1;

            noeud->scoreTotal += temp->scoreTotal;
            noeud->n++;
        }

        return;
    }

    double meilleurUcb1 = -1000000.0;
    NoeudMCTS* meilleurNoeud;

    for (int i = 0; i < noeud->nbEnfants; i++) {
        NoeudMCTS* temp = noeud->enfants[i];

        double ucb1Temp = ucb1(temp, racine->n);

        if (ucb1Temp > meilleurUcb1) {
            meilleurUcb1 = ucb1Temp;
            meilleurNoeud = temp;
        }
    }
    
    double scoreAvantParcours = meilleurNoeud->scoreTotal;
    int nAvantParcours = meilleurNoeud->n;

    parcoursMCTS(meilleurNoeud, evaluation, profondeurMax, racine);

    noeud->scoreTotal += meilleurNoeud->scoreTotal - scoreAvantParcours;
    noeud->n += meilleurNoeud->n - nAvantParcours;

    return;
}


Coup* choisirMeilleurCoupMCTS(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    NoeudMCTS* racine = creerNoeudMCTS(jeu, -1, -1, 0, 0.0);

    double limiteTempsDouble = (double)limiteTempsInt - 0.01; 

    Temps t;
    t.debut = clock();
    t.limiteTemps = limiteTempsDouble; 
    t.estFinTemps = false;

    while(!t.estFinTemps) {

        parcoursMCTS(racine, evaluation, 50, racine);

        
    }

    double meilleurUcb1 = -1000000.0;
    NoeudMCTS* meilleurNoeud;

    for (int i = 0; i < racine->nbEnfants; i++) {
        NoeudMCTS* temp = racine->enfants[i];

        double ucb1Temp = ucb1(temp, racine->n);

        if (ucb1Temp > meilleurUcb1) {
            meilleurUcb1 = ucb1Temp;
            meilleurNoeud = temp;
        }
    }

    return meilleurNoeud->coup;
}