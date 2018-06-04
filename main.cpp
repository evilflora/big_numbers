/* SCHWARTZ Guillaume - 29/05/2018
// Links :
// Optimization : https://www.codeproject.com/Articles/6154/Writing-Efficient-C-and-C-Code-Optimization
// Optimization : https://www-s.acm.illinois.edu/webmonkeys/book/c_guide/index.html
// Optimization : http://www.agner.org/optimize/optimizing_cpp.pdf
// Operators    : https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/cplr318.htm
*/
#include <iostream>
#include <string.h>

#define MAX 100
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
  // constructors
  Currency();
  Currency(const char*);
  // functions
  void add(const char*);
  void subtract(const char*);
  void multiply(const char*);
  void modulo(const char*);
  void reset();
  void show();
  char* get();
  void set(const char*);
  uint64_t index(); /* retourne l'index du premier chiffre != 0 en partant de la gauche */
  // operators
  friend Currency& operator++(Currency&, const int);
  Currency  operator+ (Currency&);
  Currency  operator* (Currency&);
  Currency  operator- (Currency&);
  Currency  operator% (Currency&);
  bool      operator<=(Currency&);
  bool      operator>=(Currency&);
  bool      operator==(Currency&);
};

//---------------------------------------------------------

Currency::Currency() {
  reset();
}

Currency::Currency(const char* value) {
  set(value);
}

//---------------------------------------------------------

void Currency::add(const char* value) {
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
  _index = ( i > _index ? i : _index); // le nouvel index est celui qu'on vient d'atteindre dans la boucle
}

void Currency::subtract(const char* value) {
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
  for(i = MAX-_index; i < MAX; i++) { //fix du décalage de l'index avec la soustration, temporaire // todo
    if (bigValue[i] != '0'){
        _index = MAX - i;
        return;
    }
    if(i == MAX - 1) _index = 1;
  }
}

void Currency::multiply(const char* value) {
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
  for(i = MAX-_index; i < MAX; i++) { //fix du décalage de l'index avec la soustration, temporaire // todo
    if (bigValue[i] != '0'){
        _index = MAX - i;
        return;
    }
    if(i == MAX - 1) _index = 1;
  }
}

void Currency::modulo(const char* value) {
  bool sub = true;
  uint64_t max = strlen(value);
  
  while(sub) {
    if(max > _index) { // si la longueur du chiffre à soustraire est supérieure à a longueur du chiffre, chiffre = le plus petit
      sub = false; // donc on ne soustrait rien
    } else if (max < _index) { // si la longueur du chiffre à soustraire est inférieur à a longueur du chiffre, chiffre à soustraire = le plus petit
      subtract(value); // donc on soustrait
    } else { // longueur indentique on doit donc trouver qui est le plus grand entre les deux
       short equal = strncmp(bigValue+index(),value,MAX);
       if (equal >= 0) {
           subtract(value);
       } else {
           sub = false;
       }
    }
  }
}

void Currency::show() {
  printf("Current %.*s\n",MAX-index(),bigValue+index());
}

void Currency::reset() {
  memset(bigValue,'0',MAX);
  _index = 0;
}

char* Currency::get(){
  return bigValue+index();
}

void Currency::set(const char* value) {
  reset(); // init du tableau à '0'
  _index = strlen(value); // longueur de la valeur à copier
  memcpy(bigValue+MAX-_index,value,_index); // copie de la valeur 
}

uint64_t Currency::index(){
  return MAX-_index;
  /*uint64_t i = 0;
  for(i = 0; i < MAX; i++) {
    if (bigValue[i] != '0') return i;
  }
  return MAX-1;*/
}

//---------------------------------------------------------

bool Currency::operator<=(Currency& v1) {
  // return true si gauche <= droite
  // return false si gauche > droite
  if(this->_index > v1._index) { // si la longueur du chiffre de gauche est inférieure à a longueur du chiffre de droite, gauche = le plus petit
    return false;
   } else if (this->_index < v1._index) { // si la longueur du chiffre de gauche est supérieure à a longueur du chiffre de droite, droite = le plus petit
    return true;
  } else { // longueur indentique on doit donc trouver qui est le plus grand entre les deux
    short equal = strncmp(this->bigValue+this->index(),v1.bigValue+v1.index(),MAX);
    if (equal > 0) {
        return false;
    } else if (equal < 0) {
        return true;
    } else {
        return true;
    }
  }
  return false;
} // good

bool Currency::operator>=(Currency& v1) {
  if(this->index() > v1.index()) { // si la longueur du chiffre de gauche est supérieure à a longueur du chiffre de droite, droite = le plus petit
    return false;
   } else if (v1.index() < this->index()) { // si la longueur du chiffre de gauche est inférieur à a longueur du chiffre de droite, gauche = le plus petit
    return true;
  } else { // longueur indentique on doit donc trouver qui est le plus grand entre les deux
    short equal = strncmp(v1.bigValue+v1.index(),this->bigValue+this->index(),MAX);
    if (equal > 0) {
        return false;
    } else {
        return true;
    }
  }
  return false;
} // à vérifier, les retour et conditions sont fuasses

bool Currency::operator==(Currency& v1) {
  if (v1._index == 1 && this->_index == 1 ) { // longueur indentique, on compare s'ils sont égaux
    if (v1.bigValue[MAX-1] == '0' && this->bigValue[MAX-1] == '0') return true;
    return false;
  } else return false;
  return false;
} // n'est pas un vrai == on ne cherche qu'à savoir si c'est égal à 0

Currency Currency::operator+(Currency& v1) {
  Currency a(this->get());
  a.add(v1.get());
  return a;
} // good

Currency Currency::operator*(Currency& v1) {
  Currency a(this->get());
  a.multiply(v1.get());
  return a;
} // good

Currency Currency::operator-(Currency& v1) {
  Currency a(this->get());
  a.subtract(v1.get());
  return a;
} // yeah why not

Currency Currency::operator%(Currency& v1) {
  Currency a(this->get());
  a.modulo(v1.get());
  return a;
} // good but not perfect

Currency& operator++(Currency& v1, int v2) {
  v1.add("1");
  return v1;
} // good

//---------------------------------------------------------

int main() {
  #if DEBUG
  clock_t begin = clock();
  #endif
  
  uint64_t k = 10000;
  Currency i = "2"; // ne peux pas être < 2 sinon boucle infinie
  Currency j = "0";
  Currency zero = "0";
  Currency prime = "0";
  
  while(k--) {
    bool isPrime=true;
    for(j="2"; j*j<=i; j++){
      if(i%j==zero) {
  	    isPrime=false;
  	    break;
      }
    }
    if(isPrime) prime.set(i.get());
    i++;
  }
  
  prime.show();
  
  #if DEBUG
  clock_t end = clock();
  printf("Duration = %f s\n",(double)(end - begin) / CLOCKS_PER_SEC);
  #endif
}
