#include <stdio.h>
#include "jeu.h"

Jeu* initJeu(bool joueurMachine) {
    Jeu* jeu = (Jeu*)malloc(sizeof(Jeu));

    for (int i = 0; i < 16; i++) {
        jeu->rouge[i] = 2;
        jeu->bleu[i] = 2;
        jeu->transparent[i] = 2;
    }

    jeu->score[0] = 0;
    jeu->score[1] = 0;

    jeu->joueurMachine = joueurMachine;
    jeu->joueurActuel = 0;

    return jeu;
}


void ajouterGraine(Jeu* jeu, int trou, int couleur) {
    if (couleur == 0) {
        jeu->rouge[trou]++;
    } else if (couleur == 1) {
        jeu->bleu[trou]++;
    } else {
        jeu->transparent[trou]++;
    }
}


void retirerGraine(Jeu* jeu, int trou, int couleur) {
    if (couleur == 0 && jeu->rouge[trou] > 0) {
        jeu->rouge[trou]--;
    } else if (couleur == 1 && jeu->bleu[trou] > 0) {
        jeu->bleu[trou]--;
    } else if (couleur == 2 && jeu->transparent[trou] > 0) {
        jeu->transparent[trou]--;
    }
}


int prendreGraines(Jeu* jeu, int trou, int couleur) {
    int graines = 0;
    if (couleur == 0) {
        graines = jeu->rouge[trou];
        jeu->rouge[trou] = 0;
    } else if (couleur == 1) {
        graines = jeu->bleu[trou];
        jeu->bleu[trou] = 0;
    } else {
        graines = jeu->transparent[trou];
        jeu->transparent[trou] = 0;
    }
    return graines;
}


int distribuerGraines(Jeu* jeu, int trou, int couleur) {
    int grainesCouleur = 0;
    int grainesTransparente = 0;
    int step = (couleur%2) + 1;

    grainesCouleur = prendreGraines(jeu, trou, couleur%2);

    if (couleur >= 2) {
        grainesTransparente = prendreGraines(jeu, trou, couleur);
    }
    
    int trouActuel = trou+1;

    while (grainesTransparente > 0) {
        ajouterGraine(jeu, trouActuel, 2);
        grainesTransparente--;
        trouActuel = (trouActuel + step) % 16;
    }
    while (grainesCouleur > 0) {
        ajouterGraine(jeu, trouActuel, couleur%2);
        grainesCouleur--;
        trouActuel = (trouActuel + step) % 16;
    }

    trouActuel = (trouActuel - step + 16) % 16;
    return trouActuel;
}


void viderTrou(Jeu* jeu, int trou) {
    jeu->rouge[trou] = 0;
    jeu->bleu[trou] = 0;
    jeu->transparent[trou] = 0;
}


int recupererNbGrainesTotal(Jeu* jeu, int trou) {
    return jeu->rouge[trou] + jeu->bleu[trou] + jeu->transparent[trou];
}


void capturerGraines(Jeu* jeu, int trou) {
    int trouActuel = trou;
    int nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);

    while (nbGrainesTotal == 2 || nbGrainesTotal == 3) {
        viderTrou(jeu, trouActuel);
        jeu->score[jeu->joueurActuel] += nbGrainesTotal;
    
        trouActuel = (trouActuel-1 + 16) % 16;
        nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);
    }
}


void lireCoup(Jeu* jeu) {
    printf("joueur %d \n", jeu->joueurActuel);
    printf("Entrez le coup : ");
    fgets(jeu->coup, 5, stdin);
}


void jouerTour(Jeu* jeu) {
    lireCoup(jeu);
    char* coup = jeu->coup;

    bool estTransparent = strchr(coup, 'T') != NULL;
    char couleurChar = toupper(jeu->coup[strlen(coup) - 1]);

    int trou;

    char tmp[3];
    if (estTransparent) {
        strncpy(tmp, coup, strlen(coup) - 2);
        tmp[strlen(coup) - 2] = '\0';
    } else {
        strncpy(tmp, coup, strlen(coup) - 1);
        tmp[strlen(coup) - 1] = '\0';
    }

    trou = atoi(tmp) - 1;
    int couleur = (estTransparent*2) + ((couleurChar == 'R')? 0 : 1);

    distribuerGraines(jeu, trou, couleur);

    capturerGraines(jeu, trou);
}

void afficherTour(Jeu* jeu) {

}

void jouerPartie() {
    Jeu* jeu = initJeu(0);

    while (96 - (jeu->score[0] + jeu->score[1]) >= 10) {
        afficherTour(jeu);

        jouerTour(jeu);

        jeu->joueurActuel = (jeu->joueurActuel+1) % 2;
    }
}

