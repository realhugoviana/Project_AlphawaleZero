#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"
#include "mcts.h"


#define DEBUG_MODE 1   // mettre 0 pour désactiver

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

    for (int trou = parent->jeu->joueurActuel; trou < 16; trou+= 2) {
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


// Fonction de plongeon pour le MCTS, génère un coup aléatoire jusqu'à une certaine profondeur et évalue la position
double plongeon(Jeu* jeu, double (*evaluation)(Jeu*), int profondeurMax, int joueurRacine) {
    Jeu* jeuCopie = copierJeu(jeu);
    DEBUG_PRINT("Début plongeon: joueurRacine=%d, profondeurMax=%d\n", joueurRacine, profondeurMax);

    for (int d = 0; d < profondeurMax; d++) {
        if (estFinPartie(jeuCopie)) {
            DEBUG_PRINT("Fin de partie atteinte à la profondeur %d\n", d);
            break;
        }

        Coup* coupAleatoire = creerCoupAleatoire(jeuCopie);
        DEBUG_PRINT("Profondeur %d: coup aléatoire (trou=%d, couleur=%d)\n", d, coupAleatoire->trou, coupAleatoire->couleur);

        jouerCoup(jeuCopie, coupAleatoire);
        libererCoup(coupAleatoire);
    }

    double score = evaluation(jeuCopie);
    DEBUG_PRINT("Score évalué après plongeon: %.2f\n", score);

    libererJeu(jeuCopie);

    return (joueurRacine == 0 ? score : -score);
}

double ucb1(NoeudMCTS* noeud, int N) {
    if (noeud->n == 0) {
        return 10000.0; // Priorité maximale pour les noeuds non explorés
    }

    double moyenneScore = noeud->scoreTotal / noeud->n;
    double exploration = 1.414 * sqrt(log(N) / noeud->n);

    return moyenneScore + exploration;
}

int indiceMeilleurUcb1(NoeudMCTS* noeud, int N) {
    if (noeud->nbEnfants == 0) 
        return -1;

    double meilleurUcb1 = -1000000.0;
    int indiceMeilleurNoeud = -1;

    for (int i = 0; i < noeud->nbEnfants; i++) {
        NoeudMCTS* temp = noeud->enfants[i];

        double ucb1Temp = ucb1(temp, N);

        if (ucb1Temp > meilleurUcb1) {
            meilleurUcb1 = ucb1Temp;
            indiceMeilleurNoeud = i;
        }
    }

    return indiceMeilleurNoeud;
}

void parcoursMCTS(NoeudMCTS* noeud, double (*evaluation)(Jeu*), int profondeurMax, int joueurActuel) {
    DEBUG_PRINT("Entrée dans parcoursMCTS: coup(trou=%d, couleur=%d), n=%d, scoreTotal=%.2f, nbEnfants=%d\n",
                noeud->coup->trou, noeud->coup->couleur, noeud->n, noeud->scoreTotal, noeud->nbEnfants);

    if (noeud->nbEnfants == 0) {
        DEBUG_PRINT("Noeud feuille ou non développé. Génération des enfants...\n");
        genererNoeudsMCTSEnfants(noeud); // aléatoire

        DEBUG_PRINT("Nombre d'enfants générés: %d\n", noeud->nbEnfants);

        for (int i = 0; i < noeud->nbEnfants; i++) {
            NoeudMCTS* temp = noeud->enfants[i];
            DEBUG_PRINT("Plongeon sur enfant %d: coup(trou=%d, couleur=%d)\n", i, temp->coup->trou, temp->coup->couleur);

            temp->scoreTotal = plongeon(temp->jeu, evaluation, profondeurMax, joueurActuel);
            temp->n = 1;

            DEBUG_PRINT("Score après plongeon: %.2f\n", temp->scoreTotal);

            noeud->scoreTotal += temp->scoreTotal;
            noeud->n++;

            DEBUG_PRINT("Mise à jour du noeud parent: n=%d, scoreTotal=%.2f\n", noeud->n, noeud->scoreTotal);
        }

        return;
    }

    int indiceMeilleur = indiceMeilleurUcb1(noeud, noeud->n);
    NoeudMCTS* meilleurNoeud = noeud->enfants[indiceMeilleur];
    DEBUG_PRINT("Choix du meilleur enfant via UCB1: indice=%d, coup(trou=%d, couleur=%d)\n",
                indiceMeilleur, meilleurNoeud->coup->trou, meilleurNoeud->coup->couleur);

    double scoreAvantParcours = meilleurNoeud->scoreTotal;
    int nAvantParcours = meilleurNoeud->n;

    parcoursMCTS(meilleurNoeud, evaluation, profondeurMax, joueurActuel);

    noeud->scoreTotal += meilleurNoeud->scoreTotal - scoreAvantParcours;
    noeud->n += meilleurNoeud->n - nAvantParcours;

    DEBUG_PRINT("Retour du parcours: noeud coup(trou=%d, couleur=%d), n=%d, scoreTotal=%.2f\n",
                noeud->coup->trou, noeud->coup->couleur, noeud->n, noeud->scoreTotal);

    return;
}


Coup* choisirMeilleurCoupMCTS(Jeu* jeu, int limiteTempsInt, double (*minimax)(Jeu*, int, double, double, bool, double (*)(Jeu*)), double (*evaluation)(Jeu*)) {
    NoeudMCTS* racine = creerNoeudMCTS(jeu, -1, -1, 0, 0.0);

    double limiteTempsDouble = (double)limiteTempsInt - 0.01; 

    Temps t;
    jeu->t.debut = clock();
    jeu->t.limiteTemps = limiteTempsDouble; 
    jeu->t.estFinTemps = false;
    int counter = 0;
    while(!t.estFinTemps) {
        DEBUG_PRINT("Début parcours MCTS\n");
        parcoursMCTS(racine, evaluation, 10, racine->jeu->joueurActuel);
        DEBUG_PRINT("Fin parcours MCTS\n");
        counter++;
        DEBUG_PRINT("Nombre de parcours effectués : %d\n", counter);

        verifierFinDuTemps(&jeu->t);
    }

    double meilleurScore = -100000.0;
    NoeudMCTS* meilleurNoeud;

    for (int i = 0; i < racine->nbEnfants; i++) {
        NoeudMCTS* temp = racine->enfants[i];

        double scoreMoyen = temp->scoreTotal / temp->n;

        if (scoreMoyen > meilleurScore) {
            meilleurScore = scoreMoyen;
            meilleurNoeud = temp;
        }
    }

    Coup* meilleurCoup = creerCoup(meilleurNoeud->coup->trou, meilleurNoeud->coup->couleur);

    libererNoeudMCTS(racine);

    return meilleurCoup;
}