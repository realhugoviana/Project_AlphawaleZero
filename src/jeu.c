#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>

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
    printf("[DEBUG] Début de distribuerGraines(trou = %d, couleur = %d)\n", trou, couleur);

    int grainesCouleur = 0;
    int grainesTransparente = 0;
    int step = (couleur % 2) + 1;

    printf("[DEBUG] Step pour la distribution = %d\n", step);

    grainesCouleur = prendreGraines(jeu, trou, couleur % 2);
    printf("[DEBUG] Graines de couleur %d prises : %d\n", couleur % 2, grainesCouleur);

    if (couleur >= 2) {
        grainesTransparente = prendreGraines(jeu, trou, couleur);
        printf("[DEBUG] Graines transparentes prises : %d\n", grainesTransparente);
    }

    int trouActuel = (trou + 1) % 16;
    printf("[DEBUG] Distribution commence au trou %d\n", trouActuel);

    // Distribution des graines transparentes
    while (grainesTransparente > 0) {
        if (trouActuel == trou) {
            // Sauter le trou d'origine
            trouActuel = (trouActuel + step) % 16;
        }

        ajouterGraine(jeu, trouActuel, 2);
        printf("[DEBUG] Ajout d'une graine transparente au trou %d (restantes = %d)\n",
               trouActuel, grainesTransparente - 1);
        grainesTransparente--;
        trouActuel = (trouActuel + step) % 16;
    }

    // Distribution des graines de couleur
    while (grainesCouleur > 0) {
        if (trouActuel == trou) {
            // Sauter le trou d'origine
            trouActuel = (trouActuel + step) % 16;
        }

        ajouterGraine(jeu, trouActuel, couleur % 2);
        printf("[DEBUG] Ajout d'une graine de couleur %d au trou %d (restantes = %d)\n",
               couleur % 2, trouActuel, grainesCouleur - 1);
        grainesCouleur--;
        trouActuel = (trouActuel + step) % 16;
    }

    // Retour au dernier trou distribué
    trouActuel = (trouActuel - step + 16) % 16;
    printf("[DEBUG] Dernier trou distribué : %d\n", trouActuel);
    printf("[DEBUG] Fin de distribuerGraines()\n\n");

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


bool estAffame(Jeu* jeu, int trou) {
    while (trou < 16) {
        if (recupererNbGrainesTotal(jeu, trou) > 0) {
            return false; 
        }
        trou += 2;
    }
    return true; 
    
}

void capturerGraines(Jeu* jeu, int trou) {
    printf("[DEBUG] Début de capturerGraines(trou = %d)\n", trou);

    int trouActuel = trou;
    int nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);
    printf("[DEBUG] Nombre de graines initial dans le trou %d : %d\n", trouActuel, nbGrainesTotal);

    while (nbGrainesTotal == 2 || nbGrainesTotal == 3) {
        printf("[DEBUG] → Trou %d contient %d graines → capture !\n", trouActuel, nbGrainesTotal);

        // Vider le trou et mettre à jour le score
        viderTrou(jeu, trouActuel);
        jeu->score[jeu->joueurActuel] += nbGrainesTotal;

        printf("[DEBUG]    - Trou %d vidé.\n", trouActuel);
        printf("[DEBUG]    - Score joueur %d = %d\n", 
               jeu->joueurActuel, jeu->score[jeu->joueurActuel]);

        // Passer au trou précédent (boucle circulaire)
        int trouPrecedent = trouActuel;
        trouActuel = (trouActuel - 1 + 16) % 16;
        nbGrainesTotal = recupererNbGrainesTotal(jeu, trouActuel);

        printf("[DEBUG]    - Passage du trou %d au trou %d (nbGraines = %d)\n",
               trouPrecedent, trouActuel, nbGrainesTotal);
    }


    if (estAffame(jeu, jeu->joueurActuel)) {
        printf("Le joueur %d a été affamé.\n",
               jeu->joueurActuel+1);

        // donner les graines restantes à joueur
        jeu->score[jeu->joueurActuel] += 96 - (jeu->score[0] + jeu->score[1]);
    }


    printf("[DEBUG] Fin de la boucle de capture. Condition non remplie (trou %d : %d graines)\n",
           trouActuel, nbGrainesTotal);
    printf("[DEBUG] Fin de capturerGraines()\n\n");
}


void lireCoup(Jeu* jeu) {
    printf("joueur %d \n", jeu->joueurActuel+1);
    printf("Entrez le coup : ");
    fgets(jeu->coup, 5, stdin);

    size_t len = strlen(jeu->coup);
    if (len > 0 && jeu->coup[len - 1] == '\n') {
        jeu->coup[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}


void jouerTour(Jeu* jeu) {
    printf("[DEBUG] Début de jouerTour()\n");

    lireCoup(jeu);
    char* coup = jeu->coup;
    printf("[DEBUG] Coup lu : '%s'\n", coup);

    bool estTransparent = strchr(coup, 'T') != NULL || strchr(coup, 't') != NULL;
    printf("[DEBUG] Coup transparent ? %s\n", estTransparent ? "oui" : "non");

    char couleurChar = toupper(jeu->coup[strlen(coup) - 1]);
    printf("[DEBUG] Caractère couleur lu : '%c'\n", couleurChar);

    int trou;
    char tmp[3];

    if (estTransparent) {
        strncpy(tmp, coup, strlen(coup) - 2);
        tmp[strlen(coup) - 2] = '\0';
        printf("[DEBUG] Extraction coup transparent, tmp = '%s'\n", tmp);
    } else {
        strncpy(tmp, coup, strlen(coup) - 1);
        tmp[strlen(coup) - 1] = '\0';
        printf("[DEBUG] Extraction coup normal, tmp = '%s'\n", tmp);
    }

    trou = atoi(tmp) - 1;
    printf("[DEBUG] Trou choisi : %d (index 0-based)\n", trou);

    int couleur = (estTransparent * 2) + ((couleurChar == 'R') ? 0 : 1);
    printf("[DEBUG] Couleur calculée : %d\n", couleur);
    printf("         -> 0 = Rouge, 1 = Bleu, 2 = Transparent Rouge, 3 = Transparent Bleu\n");

    printf("[DEBUG] Appel de distribuerGraines(jeu, %d, %d)\n", trou, couleur);
    trou = distribuerGraines(jeu, trou, couleur);

    printf("[DEBUG] Appel de capturerGraines(jeu, %d)\n", trou);
    capturerGraines(jeu, trou);

    printf("[DEBUG] Fin de jouerTour()\n\n");
}


void afficherJeu(Jeu* jeu) {
    printf("=== ÉTAT DU JEU ===\n\n");

    for (int i = 0; i < 16; i++) {
        printf("trou %2d: R: %2d | B: %2d | T: %2d \n", i+1, jeu->rouge[i], jeu->bleu[i], jeu->transparent[i]);
    }
    printf("\n\n");

    printf("Dernier coup joué : %s\n", jeu->coup);
    printf("Score : 1 = %2d | 2 = %2d\n", jeu->score[0], jeu->score[1]);
    printf("Joueur actuel : %s\n", jeu->joueurActuel ? "2" : "1");

    printf("====================\n");
}


bool estFinPartie(Jeu* jeu) {
    if (jeu->score[0] >= 49) {
        printf("Le joueur 1 a gagné !\n");
        return true;
    } else if (jeu->score[1] >= 49) {
        printf("Le joueur 2 a gagné !\n");
        return true;
    } else if (jeu->score[0] == 40 && jeu->score[1] == 40) {
        printf("Match nul !\n");
        return true;
    } else if (96 - (jeu->score[0] + jeu->score[1]) < 10) {
        printf("Moins de 10 graines restantes.\n");
        if (jeu->score[0] > jeu->score[1]) {
            printf("Le joueur 1 a gagné !\n");
        } else {
            printf("Le joueur 2 a gagné !\n");
        } 
        return true;
    } else {
        return false;
    }
}


void copierJeu(const Jeu* src, Jeu* dst) {
    for (int i = 0; i < 16; i++) {
        dst->rouge[i]       = src->rouge[i];
        dst->bleu[i]        = src->bleu[i];
        dst->transparent[i] = src->transparent[i];
    }

    dst->score[0]     = src->score[0];
    dst->score[1]     = src->score[1];

    dst->joueurMachine = src->joueurMachine;
    dst->joueurActuel = src->joueurActuel;

    strcpy(dst->coup, src->coup);
}


// factoriser afficherCoups
void afficherCoup(Jeu* jeu, int i, bool joueur, int couleur) {
    Jeu tmp;
    copierJeu(jeu, &tmp);

    int scoreAvant = tmp.score[joueur];

    int trou = distribuerGraines(&tmp, i, couleur);
    capturerGraines(&tmp, trou);

    if (couleur == 0) {
        printf("   Coup joué : %dR\n", i + 1);
    } else if (couleur == 1) {
        printf("   Coup joué : %dB\n", i + 1);
    } else if (couleur == 2) {
        printf("   Coup joué : %dTR\n", i + 1);
    } else {
        printf("   Coup joué : %dTB\n", i + 1);
    }
    printf("   Graines capturées : %d\n\n", tmp.score[joueur] - scoreAvant);

}


// affiche tout les possibles coups jouables
void afficherCoups(Jeu* jeu) {
    printf("Coups jouables pour le joueur %d :\n", jeu->joueurActuel + 1);
    bool joueur = jeu->joueurActuel;

    for (int i = joueur; i < 16; i += 2) {
        if (recupererNbGrainesTotal(jeu, i) > 0) {
            printf(" - Trou %d\n", i + 1);
            
            if (jeu->rouge[i] > 0) {
                afficherCoup(jeu, i, joueur, 0);
            } 

            if (jeu->bleu[i] > 0) {
                afficherCoup(jeu, i, joueur, 1);
            }
            if (jeu->transparent[i] > 0) {
                afficherCoup(jeu, i, joueur, 2);

                afficherCoup(jeu, i, joueur, 3);
            }
        } 
    }
}


void jouerPartie() {
    Jeu* jeu = initJeu(0);

    afficherJeu(jeu);
    while (!estFinPartie(jeu)) {
        afficherCoups(jeu);
        jouerTour(jeu);

        jeu->joueurActuel+=1;
        afficherJeu(jeu);
    }

    printf("Partie terminée.\n");
}

