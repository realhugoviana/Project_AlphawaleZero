#ifndef JEU_H
#define JEU_H

#include <stdbool.h>

int distribuerGraines(int trou, int couleur);

void viderTrou(int trou);

void capturerGraines(int trou, bool joueur);

void tour(bool joueur);

void jouerPartie();

#endif // JEU_H