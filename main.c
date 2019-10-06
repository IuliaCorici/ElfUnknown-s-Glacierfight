// Copyright 2019 Corici- Iulia Stefania 314CA
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define NMax 20

// struct pt loc de joaca
// A altitudinea
// B dimensiunea manusilor
// exist marcheaza existenta unui elf pe ghetar
typedef struct PLAYGROUND {
  int A, B, exist;
} PLAYGROUND;

// struct pt spiridusi
// x linia pe care se afla
// y coloana pe care se afla
// hp nivelul de uscare
// stamina energia elfului
// dmg dimensiunea manusilor
// invinsi cati spiridusi a invins elful
typedef struct Spiridusi {
  char *nume;
  int x, y, hp, stamina, dmg, invinsi;
} SPIRIDUS;

// functie pt batalia dintre doi elfi
void batalie(SPIRIDUS **elf, int id, int P, FILE *out) {
  // unu - elful cu stamina mai mare
  // doi- retine id ul celuilalt elf din casuta
  int i = 0, unu, doi;
  for ( i = 0 ; i < P ; i++ ) {
    // daca nu e acelasi elf si se afla in aceeasi casuta
    // si nu e ud cel care intra in casuta
    if ( i != id && (*elf)[id].y == (*elf)[i].y &&
     (*elf)[id].x == (*elf)[i].x && (*elf)[i].hp > 0 ) {
      if ( (*elf)[id].stamina >= (*elf)[i].stamina ) {
        unu = id;
        doi = i;
      } else {
        unu = i;
        doi = id;
      }
      // cat timp elfii sunt uscati se desfasoara bataia cu bulgari
      while ( (*elf)[unu].hp > 0 && (*elf)[doi].hp > 0 ) {
        (*elf)[doi].hp -= (*elf)[unu].dmg;
        // daca s-a udat elful al doilea
        // se maresc invinsii primului si isi mareste stamina
        // iar cel de-al doilea e trimis acasa
        if ( (*elf)[doi].hp <= 0 ) {
          (*elf)[unu].invinsi++;
          (*elf)[unu].stamina += (*elf)[doi].stamina;
          fprintf(out , "%s sent %s back home.\n", (*elf)[unu].nume,
            (*elf)[doi].nume);
          return;
        }
        (*elf)[unu].hp -= (*elf)[doi].dmg;
        // analog
        if ( (*elf)[unu].hp <= 0 ) {
          (*elf)[doi].invinsi++;
          (*elf)[doi].stamina += (*elf)[unu].stamina;
          fprintf(out , "%s sent %s back home.\n", (*elf)[doi].nume,
            (*elf)[unu].nume);
          return;
        }
      }
    }
  }
}

// functie pt mutarea elfilor
void muta_elf(PLAYGROUND ***ghetar, SPIRIDUS **elf, FILE *in, FILE *out, int R,
int P, int *vii) {
  // id numarul elfului
  // x_urm, y_urm pozitiile urmatoare
  // diferenta de altitudine
  // directii - directiile pe care le primeste
  int id, i = 0, x_urm, y_urm, diferenta;
  char directii[NMax];
  fscanf(in, "%d %s", &id, directii);
  // cat timp e cel putin un elf pe ghetar si mai sunt directii
  // de citit si elful nu e ud
  while ( (*elf)[id].hp > 0 && directii[i] != '\0' && (*vii) > 1 ) {
    x_urm = (*elf)[id].x;
    y_urm = (*elf)[id].y;
    // pt dreapta
    if ( directii[i] == 'R' ) {
      y_urm++;
    }
    // pt stanga
    if ( directii[i] == 'L' ) {
      y_urm--;
    }
    // pt sus
    if ( directii[i] == 'U' ) {
      x_urm--;
    }
    // pt jos
    if ( directii[i] == 'D' ) {
      x_urm++;
    }
    // daca e pe harta ghetarului( in matrice)
    if ( x_urm < 0 || x_urm > 2 * R || y_urm < 0 || y_urm > 2 * R ) {
      (*ghetar)[(*elf)[id].x][(*elf)[id].y].exist = 0;
      fprintf(out , "%s fell off the glacier.\n", (*elf)[id].nume);
      (*vii)--;
      // a cazut de pe ghetar, s-a udat, scade numarul elfilor vii
      (*elf)[id].hp = 0;
      return;
    } else {
      diferenta = (*ghetar)[(*elf)[id].x][(*elf)[id].y].A -
      (*ghetar)[x_urm][y_urm].A;
      if ( diferenta < 0 ) {
        // modulul diferentei de altitudine
        diferenta = (-1) * diferenta;
      }

      if ( (*elf)[id].stamina >= diferenta ) {
        (*elf)[id].stamina -= diferenta;
        // se modifica stamina pt ca a facut mutarea
        (*ghetar)[(*elf)[id].x][(*elf)[id].y].exist = 0;
        // s-a ocupat casuta
        (*elf)[id].x = x_urm;
        (*elf)[id].y = y_urm;
        if ( ((*elf)[id].x - R) * ((*elf)[id].x - R) + ((*elf)[id].y - R) *
          ((*elf)[id].y - R) > R * R ) {
          // elful nu e pe ghetar, deci a cazut si s-a udat
          fprintf(out , "%s fell off the glacier.\n", (*elf)[id].nume);
          (*ghetar)[(*elf)[id].x][(*elf)[id].y].exist = 0;
          (*vii)--;
          (*elf)[id].hp = 0;
          return;
        }
        // se ia manusile daca au dimensiune mai mare
        if ( (*ghetar)[(*elf)[id].x][(*elf)[id].y].B > (*elf)[id].dmg ) {
          int aux = (*ghetar)[(*elf)[id].x][(*elf)[id].y].B;
          (*ghetar)[(*elf)[id].x][(*elf)[id].y].B = (*elf)[id].dmg;
          (*elf)[id].dmg = aux;
        }
        // daca casuta e ocupata
        // se apeleaza functia de batalie intre elfi
        if ( (*ghetar)[x_urm][y_urm].exist == 1 ) {
          (*vii)--;
          batalie(&(*elf), id, P, out);
        } else {
          // se ocupa casuta pt ca a fost libera
          (*ghetar)[(*elf)[id].x][(*elf)[id].y].exist = 1;
        }
      }
    }
    i++;
  }
}

// functie pt furtuna (snowstorm)
void furtuna(SPIRIDUS **elf, PLAYGROUND ***ghetar, int P, FILE *in,
FILE *out, int *vii) {
  // R raza ghetarului
  // x pt linie
  // y pt coloana
  // se shifteaza la dreapta pt a obtine componentele
  int storm, x, y, biti = 255, R, i, dmg;
  fscanf(in , "%d", &storm);
  x = storm & biti;
  storm = (storm >> 8);
  y = storm & biti;
  storm = (storm >> 8);
  R = storm & biti;
  storm = (storm >> 8);
  dmg = storm & biti;
  // efectele furtunii
  for ( i = 0 ; i < P ; i++ ) {
    if ( ((*elf)[i].x - x) * ((*elf)[i].x - x) +
      ((*elf)[i].y - y) * ((*elf)[i].y - y) <= R * R ) {
      if ( (*elf)[i].hp > 0 ) {
        // scade gradul de uscare a spiridusului
        (*elf)[i].hp -= dmg;
        if ( (*elf)[i].hp <= 0 ) {
          (*ghetar)[(*elf)[i].x][(*elf)[i].y].exist = 0;
          // scade numarul celor vii
          (*vii)--;
          // spiridus lovit de furtuna
          fprintf(out , "%s was hit by snowstorm.\n", (*elf)[i].nume);
        }
      }
    }
  }
}

// pt tabela scorului final
void tabelascor(SPIRIDUS **elf, int P, FILE *out) {
  int i, j = 0, *pointer, prag, maxim, pozmax;
  int aux;
  // prag marcheaza limita dintre spiridusi uzi si uscati
  // maxim- maximul din indicii spiridusilor uscati si uzi
  // pozmax -pozitia maximului
  // aux- variabila pt sortare
  // alocare de memorie pentru un vector aditional
  pointer = malloc(P * sizeof(int));
  // stabilirea si separarea spiridusilor uzi fata de cei uscati
  for ( i = 0 ; i < P ; i++ ) {
    if ( (*elf)[i].hp > 0 ) {
      pointer[j] = i;
      j++;
    }
  }
  prag = j;
  for ( i = 0 ; i < P ; i++ ) {
    if ( (*elf)[i].hp <= 0 ) {
       pointer[j] = i;
       j++;
    }
  }
  // tabela de scor pt spiridusii uscati
  fprintf(out, "SCOREBOARD:\n");
  // metoda de sortare prin selectie si schimbare de pozitie
  // pentru uscati
  for ( i = 0 ; i < prag ; i++ ) {
    maxim = -1;
    pozmax = i;
    for ( j = i ; j < prag ; j++ ) {
      if ( maxim < (*elf)[ pointer[j] ].invinsi ) {
        maxim = (*elf)[ pointer[j] ].invinsi;
        pozmax = j;
      } else {
        if ( maxim == (*elf)[ pointer[j] ].invinsi ) {
          if ( strcmp( (*elf)[ pointer[j] ].nume,
            (*elf)[ pointer[pozmax] ].nume ) < 0 ) {
            pozmax = j;
          }
        }
      }
    }
    aux = pointer[i];
    pointer[i] = pointer[pozmax];
    pointer[pozmax] = aux;
    fprintf(out, "%s\tDRY\t%d\n", (*elf)[pointer[i]].nume,
      (*elf)[pointer[i]].invinsi);
  }
    // pentru uzi se face sortare si se printeaza tabela de scor
    for ( i = prag ; i < P ; i++ ) {
    maxim = -1;
    pozmax = i;
    for ( j = i ; j < P ; j++ ) {
      if ( maxim < (*elf)[ pointer[j] ].invinsi ) {
        maxim = (*elf)[ pointer[j] ].invinsi;
        pozmax = j;
      } else {
        if ( maxim == (*elf)[ pointer[j] ].invinsi ) {
          if ( strcmp( (*elf)[ pointer[j] ].nume,
            (*elf)[ pointer[pozmax] ].nume ) < 0 ) {
            pozmax = j;
          }
        }
      }
    }
    aux = pointer[i];
    pointer[i] = pointer[pozmax];
    pointer[pozmax] = aux;
    fprintf(out, "%s\tWET\t%d\n", (*elf)[pointer[i]].nume,
      (*elf)[pointer[i]].invinsi);
  }
  // free pentru vectorul aditional
  free(pointer);
}

// pt topirea ghetarului
void topire(PLAYGROUND ***ghetar, SPIRIDUS **elf, int *R, int P, FILE *in,
  FILE *out, int *vii ) {
  int energie, i, j;
  fscanf(in, "%d", &energie);
  for ( i = 0 ; i < P ; i++ ) {
    if ( (*elf)[i].hp > 0 ) {
      if ( (*R - 1) * (*R - 1) < ( ((*elf)[i].x - *R) * ((*elf)[i].x - *R) +
        ((*elf)[i].y - *R) * ((*elf)[i].y - *R)) ) {
        // daca prin micsoarea razei un elf a cazut
        fprintf(out, "%s got wet because of global warming.\n",
          (*elf)[i].nume );
        (*elf)[i].hp = 0;
        (*ghetar)[(*elf)[i].x][(*elf)[i].y].exist = 0;
        // casuta se elibereaza si numarul spiridusilor vii scade
        (*vii)--;
      }
      (*elf)[i].stamina = (*elf)[i].stamina + energie;
      // primeste energie daca a supravietuit
      // se modifica pozitia elfului in urma micsorarii razei
      (*elf)[i].x--;
      (*elf)[i].y--;
    }
  }

  for ( i = 0 ; i < 2 * (*R) - 1 ; i++ ) {
    for ( j = 0 ; j < 2 * (*R) - 1 ; j++ ) {
      (*ghetar)[i][j] = (*ghetar)[i + 1][j + 1];
    } // relacoarea matricei cu raza modificata
      (*ghetar)[i] = realloc((*ghetar)[i], ( 2 * (*R - 1) + 1 ) *
        sizeof(PLAYGROUND));
  }
  // eliberarea de memorie
  free((*ghetar)[*R * 2 - 1]);
  free((*ghetar)[*R * 2]);
  (*R)--;
}

int main() {
  int R, P, vii;
  char comanda[20];
  int i, j;
  // fisier de intrare
  FILE *in = fopen("snowfight.in", "rt");
  // fisier de iesire
  FILE *out = fopen("snowfight.out", "wt");
  // R raza
  // P numarul de spiridusi
  fscanf(in, "%d%d", &R, &P);
  vii = P;
  // alocarea memoriei pt ghetar si spiridus
  PLAYGROUND **ghetar = (PLAYGROUND **) malloc((2 * R + 1) *
    sizeof(PLAYGROUND *));
  for (i = 0; i < 2 * R + 1; i++) {
    ghetar[i] = (PLAYGROUND*) malloc((2 * R + 1) * sizeof(PLAYGROUND));
  }
  for (i = 0; i < 2 * R + 1; i++) {
      for (j = 0; j < 2 * R + 1 ; j++) {
          fscanf(in, "%d %d", &ghetar[i][j].A, &ghetar[i][j].B);
          ghetar[i][j].exist = 0;
      }
    }

    SPIRIDUS *elf = (SPIRIDUS*) malloc(P * sizeof(SPIRIDUS));
    for (i = 0; i < P; i++) {
      elf[i].nume = malloc(20 * sizeof(char));
      fscanf(in, "%s %d %d %d %d", elf[i].nume, &elf[i].x, &elf[i].y,
        &elf[i].hp, &elf[i].stamina);
      elf[i].invinsi = 0;
      if ((elf[i].x - R) * (elf[i].x - R) + (elf[i].y - R) * (elf[i].y - R)
       > R * R) {
        fprintf(out, "%s has missed the glacier.\n", elf[i].nume);
        vii--;
        elf[i].hp = 0;
      } else {
        ghetar[elf[i].x][elf[i].y].exist = 1;
        elf[i].dmg = ghetar[elf[i].x][elf[i].y].B;
        ghetar[elf[i].x][elf[i].y].B = 0;
       }
    }
    // se verifica ce comanda se primeste
    // cat timp macar un spiridus e viu
    while ( vii > 1 ) {
      fscanf(in, "%s", comanda);
      if ( comanda[0] == 'A' ) {
        vii = -1;
      }
      if ( strcmp(comanda, "MOVE") == 0 ) {
        muta_elf(&ghetar, &elf, in, out, R, P, &vii);
      }
      if ( strcmp(comanda, "SNOWSTORM") == 0 ) {
        furtuna(&elf, &ghetar, P, in, out, &vii);
      }
      if ( strcmp(comanda, "PRINT_SCOREBOARD") == 0 ) {
        tabelascor(&elf, P, out);
      }
      if ( strcmp(comanda, "MELTDOWN") == 0 ) {
        topire(&ghetar, &elf, &R, P, in, out, &vii);
      }
      comanda[0] = 'A';
    }
    // afiseaza castigatorul pentru elful ramas in viata
    if ( vii == 1 ) {
      for ( i = 0 ; i < P ; i++ ) {
        if ( elf[i].hp > 0 ) {
          fprintf(out , "%s has won.\n", elf[i].nume);
        }
      }
    }
    for ( i = 0 ; i <= 2 * R  ; ++i ) {
        free(ghetar[i]);
    }
    for ( i = 0 ; i < P ; ++i ) {
      free(elf[i].nume);
    }
    // se elibereaza memoria pentru ghetar si elfi
    free(ghetar);
    free(elf);
    fclose(in);
    fclose(out);
    return 0;
  }
