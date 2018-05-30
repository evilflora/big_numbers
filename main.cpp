/* SCHWARTZ Guillaume - 29/05/2018
// Links :
// https://www.codeproject.com/Articles/6154/Writing-Efficient-C-and-C-Code-Optimization
// https://www-s.acm.illinois.edu/webmonkeys/book/c_guide/index.html
// http://www.agner.org/optimize/optimizing_cpp.pdf
// todo : http://en.cppreference.com/w/cpp/language/operators
*/
#include <iostream>
#include <string.h>

#define MAX 8000000
#define DEBUG 1

#if DEBUG
#include<time.h>
#endif

using namespace std;

/* 
 * todo : https://bousk.developpez.com/cours/multi-thread-mutex/
 */

class Currency {
  protected :
  char* bigValue = new char[MAX];
  uint64_t _index;
  
  public :
  Currency();
  void add(const char*);
  void subtract(const char*);
  void multiply(const char*);
  void divide(const char*);
  void reset();
  void show();
  
  private :
  uint64_t index(); /* retourne l'index du premier chiffre != 0 en partant de la gauche */
  bool verify_content(const char* ); /* vérifie que la chaine d'entrée est composée de chiffres */
};

Currency::Currency() {
  reset();
}

void Currency::add(const char* value) {
  if(verify_content(value)) {
    uint64_t max = strlen(value);
    uint64_t i = 0;
    uint8_t offset = 0;
    uint8_t tmp = 0;
    uint8_t a,b;
    while (offset || i != max) { // tant qu'il reste un offset ou que l'on n'a pas atteind la fin de l'addition
      a = *(bigValue+MAX-i-1) - '0';
      if (offset && i == max) { // si on a un offset mais qu'on a parcouru tout le chiffre à additionner ...
        max++; // on augmente la taille pour continuer à parcourir le chiffre de base, car il reste l'offset à additionner
        b = 0; // est à 0
      } else {
        b = *(value+max-i-1) - '0';
      }
      tmp = a + b + offset; // on aditionne les deux chiffres et l'offset
      *(bigValue+MAX-i-1) = (tmp % 10) + '0'; // avec une addition entre 2 chiffres si le chiffre est supérieur à 10 on ne regarde que reste
      offset = tmp / 10; // calcul de l'offset, = 1 si >= 10 et = 0 si < 10
      i++;  // on augmente l'index
    }
    _index = ( i > _index ? i : _index);
  }
}

void Currency::subtract(const char* value) {
  if(verify_content(value)) {
    uint64_t max = strlen(value);
    uint64_t i = 0;
    uint8_t offset = 0;
    int8_t tmp = 0; // doit être signé
    uint8_t a,b;
    while (offset || i != max) { // s'il n'y a pas d'offset et qu'on a parcouru tour le nombre à soustraire alors on quitte
      a = (bigValue[MAX-i-1] - '0'); // a = chiffre i (par la droite)
      if (offset && i == max) { // si on à un offset mais qu'on a parcouru tout le chiffre à soustraire 
        max++; // on augmente la taille pour continuer à parcourir le chiffre de base, car il reste l'offset à soustraire
        b = 0; // on met b à 0 pour simplifier les calculs suivants
      } else {
        b = (value[max-i-1] - '0'); // s'il n'y a pas d'offet et qu'on n'a pas parcouru tout le chiffre à soustraire on continue normalement
      }
      tmp = a - (b + offset); // soustraction à l'index actuel
      if (tmp < 0) { // si on obtient un chiffre négatif
        tmp += 10; // on rajoute + 10, car 3 - 8 = -5 => 5
        offset = 1;// il y a donc un offset
      } else {
        offset = 0; // sinon 8 - 5 => 3 donc pas d'offset
      }
      bigValue[MAX-i-1] = tmp + '0'; // on modifie la valeur cet index
      i++; // on augmente l'index
    }
    for(i = MAX-_index; i < MAX; i++) {
     if (bigValue[i] != '0'){
         _index = MAX - i;
         break;
     }
    }
  }
}

void Currency::multiply(const char* value) {
  if(verify_content(value)) {
    uint64_t max = strlen(value);
    uint64_t max_bigvalue = MAX - index(); // longueur du big value, pour savoir le nombre de case à multiplier
    char local_value[MAX];
    uint64_t i = 0,j = 0;
    uint8_t offset = 0;
    uint8_t tmp = 0;
    bool stop = false;
    memcpy(local_value,bigValue,MAX); // on stocke la valeur de base (car on ne doit pas la perdre)
    memset(bigValue,'0',MAX);
    while (!stop) { // tant que stop est à false et défini le nombre d'additions à réaliser
      char local[max+max_bigvalue+1] = { 0 }; // la valeur local est égale à longueur du multiplié + longueur du multiplicateur + 1 qui est '\0' à la fin
      memset(local,'0',sizeof(local)-1); // case de fin du tableau à '\0' le reste à '0'
      j = 0; // init de j
      while(j != max_bigvalue) {
        tmp = ((local_value[MAX-j-1] - '0') * (value[max-i-1] - '0')) + offset; // on multiplie chaque case sans oublier l'ajout de l'offset
        offset = tmp / 10; // on calcul l'offset entre 0 au min et 9 au max
        local[max+max_bigvalue-j-i-1] = (tmp % 10) +'0'; // on met à jour la valeur
        j++; // case suivante
        if (offset && j == max_bigvalue) { // si on a parcouru le multiplicateur (donc on a fini la multiplication) et qu'il reste un offset
          local[max+max_bigvalue-j-i-1] = offset +'0'; // on le place a la bonne case (rien à faire, le j++ est déjà fait avant)
          offset = 0; // on réinitialise l'offset à cause du calcul de tmp, car on ne repasse plus dans cette boucle
        }
      }
      add(local); // on réalise l'addition
      i++; // on augmente l'index
      if (i == max) stop = true;
    }
    _index = ( i > _index ? i : _index); // -1 car il y a le '\0'
  }
}

void Currency::divide(const char* value) {
  if(verify_content(value)) {
    uint64_t max = strlen(value);
    uint64_t max_bigvalue = MAX - index(); // longueur du big value, pour savoir le nombre de case à multiplier
    uint64_t i = 0,j = 0;
    uint8_t offset = 0;
    uint8_t tmp = 0;
    bool stop = false;
    while (!stop) { // tant que stop est à false et défini le nombre d'additions à réaliser

    }
  }
} // todo

void Currency::show() {
  printf("Current %.*s\n",MAX-index(),bigValue+index());
}

void Currency::reset() {
  memset(bigValue,'0',MAX);
  _index = 0;
}

bool Currency::verify_content(const char* value) { // todo : désactivé, temporaire
  /*uint64_t max = strlen(value);
  uint64_t i;
  for(i = 0; i < max; i++) {
    if (value[i] < '0' || value[i] > '9') return false;
  }*/
  return true;
}

uint64_t Currency::index(){
  return MAX-_index;
  /*uint64_t i = 0;
  for(i = 0; i < MAX; i++) {
    if (bigValue[i] != '0') return i;
  }
  return MAX-1;*/
}

int main() {
  Currency current;

  #if DEBUG
  clock_t begin = clock();
  #endif
  
  current.add("1");
  current.add("9999999");
  current.add("9999999");
  current.add("9999999");
  current.add("199999999999");
  current.subtract("199999999999");
  current.multiply("826217485465");
  current.multiply("826217485465");
  current.show();
  
  #if DEBUG
  clock_t end = clock();
  printf("Duration = %f s\n",(double)(end - begin) / CLOCKS_PER_SEC);
  #endif
}
