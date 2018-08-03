/* SCHWARTZ Guillaume - 29/05/2018
// Links :
// Optimization : https://www.codeproject.com/Articles/6154/Writing-Efficient-C-and-C-Code-Optimization
// Optimization : https://www-s.acm.illinois.edu/webmonkeys/book/c_guide/index.html
// Optimization : http://www.agner.org/optimize/optimizing_cpp.pdf
// Optimization : https://bousk.developpez.com/cours/multi-thread-mutex/
// WTFH         : https://fr.wikipedia.org/wiki/M%C3%A9taprogrammation_avec_des_patrons 
// Operators    : https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/cplr318.htm
*/

// Tous les nombres premiers sont impair, mais tous les nombres impairs ne sont pas des nombres premier.

#include <iostream>
#include <cstring>
#include <ctime>

#define MAX 100
#define DEBUG 1

using namespace std;

int fast_compare( const char *ptr0, const char *ptr1, int len ){
  int fast = len/sizeof(size_t) + 1;
  int offset = (fast-1)*sizeof(size_t);
  int current_block = 0;

  if( len <= sizeof(size_t)){ fast = 0; }


  size_t *lptr0 = (size_t*)ptr0;
  size_t *lptr1 = (size_t*)ptr1;

  while( current_block < fast ){
    if( (lptr0[current_block] ^ lptr1[current_block] )){
      int pos;
      for(pos = current_block*sizeof(size_t); pos < len ; ++pos ){
        if( (ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0) ){
          return  (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);
          }
        }
      }

    ++current_block;
    }

  while( len > offset ){
    if( (ptr0[offset] ^ ptr1[offset] )){ 
      return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]); 
      }
    ++offset;
    }
	
	
  return 0;
}

class Currency {
  protected :
  char* bigValue = new char[MAX]; // notre gros chiffre
  uint64_t _index{}; // longueur de notre chiffre et au passage la position à partir de la droite
  
  public :
  // constructors
  Currency();
  ~Currency();
  Currency(const char*);
  // functions
  void add(const char*);
  void subtract(const char*);
  void multiply(const char*);
  void modulo(const char*);
  void loffset(const int);
  void reset();
  void show() const;
  char* get() const;
  bool isNotPrime() const;
  void set(const char*);
  uint64_t index() const; /* retourne l'index du premier chiffre != 0 en partant de la gauche */
  // operators
  Currency operator<<(int) const;
  Currency operator++(int);
  Currency operator+=(Currency&);
  Currency operator+ (Currency&) const;
  Currency operator+=(const char*);
  Currency operator+ (const char*) const;
  Currency operator*=(Currency&);
  Currency operator* (Currency&) const;
  Currency operator*=(const char*);
  Currency operator* (const char*) const;
  Currency operator-=(Currency&);
  Currency operator- (Currency&) const;
  Currency operator-=(const char*);
  Currency operator- (const char*) const;
  Currency operator% (Currency&) const;
  bool     operator<=(Currency&) const;
  bool     operator>=(Currency&) const;
  bool     operator==(Currency&) const;
};

//---------------------------------------------------------

Currency::Currency() {
  reset();
}

Currency::~Currency() = default;

Currency::Currency(const char* value) {
  set(value);
}

//---------------------------------------------------------

void Currency::add(const char* value) {
  uint64_t max = strlen(value);
  uint64_t i = 0;
  uint8_t offset = 0;
  uint8_t b;
  while (offset || i != max) { // tant qu'il reste un offset ou que l'on n'a pas atteind la fin de l'addition
    const uint8_t a = *(bigValue+MAX-i-1) - '0';
    if (offset && i == max) { // si on a un offset mais qu'on a parcouru tout le chiffre à additionner ...
      max++; // on augmente la taille pour continuer à parcourir le chiffre de base, car il reste l'offset à additionner
      b = 0; // est à 0
    } else {
      b = *(value+max-i-1) - '0';
    }
    const uint8_t tmp = a + b + offset; // on aditionne les deux chiffres et l'offset
    *(bigValue+MAX-i-1) = (tmp % 10) + '0'; // avec une addition entre 2 chiffres si le chiffre est supérieur à 10 on ne regarde que reste
    offset = tmp / 10; // calcul de l'offset, = 1 si >= 10 et = 0 si < 10
    i++;  // on augmente l'index
  }
  _index = ( i > _index ? i : _index); // recherche de l'indexe le plus grand
} // not sure

void Currency::subtract(const char* value) {
  uint64_t max = strlen(value);
  uint64_t i = 0;
  uint8_t offset = 0;
  uint8_t b;
  while (offset || i != max) { // s'il n'y a pas d'offset et qu'on a parcouru tour le nombre à soustraire alors on quitte
    const uint8_t a = (*(bigValue+MAX-i-1) - '0'); // a = chiffre i (par la droite)
    if (offset && i == max) { // si on à un offset mais qu'on a parcouru tout le chiffre à soustraire 
      max++; // on augmente la taille pour continuer à parcourir le chiffre de base, car il reste l'offset à soustraire
      b = 0; // on met b à 0 pour simplifier les calculs suivants
    } else {
      b = (*(value+max-i-1) - '0'); // s'il n'y a pas d'offet et qu'on n'a pas parcouru tout le chiffre à soustraire on continue normalement
    }
    int8_t tmp = a - (b + offset); // doit être signé !! soustraction à l'index actuel
    if (tmp < 0) { // si on obtient un chiffre négatif
      tmp += 10; // on rajoute + 10, car 3 - 8 = -5 => 5
      offset = 1;// il y a donc un offset
    } else {
      offset = 0; // sinon 8 - 5 => 3 donc pas d'offset
    }
    *(bigValue+MAX-i-1) = tmp + '0'; // on modifie la valeur cet index
    i++; // on augmente l'index
  }
  for(i = MAX-_index; i < MAX; i++) { //fix du décalage de l'index avec la soustration, temporaire // todo
    if (*(bigValue+i) != '0'){
        _index = MAX - i;
        return;
    }
    if(i == MAX - 1) _index = 1;
  }
} // todo

void Currency::multiply(const char* value) {
  const uint64_t max = strlen(value);
  const uint64_t max_bigvalue = _index; // longueur du big value, pour savoir le nombre de case à multiplier
  char local_value[MAX];
  uint64_t i = 0;
  uint8_t offset = 0;
  bool stop = false;
  memcpy(local_value,bigValue,MAX); // on stocke la valeur de base (car on ne doit pas la perdre)
  memset(bigValue,'0',MAX);
  while (!stop) { // tant que stop est à false et défini le nombre d'additions à réaliser
    char local[max+max_bigvalue+1] = { 0 }; // la valeur local est égale à longueur du multiplié + longueur du multiplicateur + 1 qui est '\0' à la fin
    memset(local,'0',sizeof(local)-1); // case de fin du tableau à '\0' le reste à '0'
    uint64_t j = 0; // init de j
    while(j != max_bigvalue) {
      const uint8_t tmp = ((*(local_value+MAX-j-1) - '0') * (*(value+max-i-1) - '0')) + offset; // on multiplie chaque case sans oublier l'ajout de l'offset
      offset = tmp / 10; // on calcul l'offset entre 0 au min et 9 au max
      *(local+max+max_bigvalue-j-i-1) = (tmp % 10) +'0'; // on met à jour la valeur
      j++; // case suivante
      if (offset && j == max_bigvalue) { // si on a parcouru le multiplicateur (donc on a fini la multiplication) et qu'il reste un offset
        *(local+max+max_bigvalue-j-i-1) = offset +'0'; // on le place a la bonne case (rien à faire, le j++ est déjà fait avant)
        offset = 0; // on réinitialise l'offset à cause du calcul de tmp, car on ne repasse plus dans cette boucle
      }
    }
    add(local); // on réalise l'addition
    i++; // on augmente l'index
    if (i == max) stop = true;
  }
  for(i = MAX-_index; i < MAX; i++) { //fix du décalage de l'index avec la soustration, temporaire // todo
    if (*(bigValue+i) != '0'){
        _index = MAX - i;
        return;
    }
    if(i == MAX - 1) _index = 1;
  }
} // todo

void Currency::modulo(const char* value) { 
  bool sub = true;
  Currency minus = value; // value est la valeur du modulo, par exemple 50 % 20 ==> 20
  uint64_t max = minus._index; // la longueur de la valeur, en reprenant la valeur precente ==> 2

  while(sub) {
    if(max > _index) { // si la longueur du modulo est supérieure à la longueur du chiffre, chiffre = le plus petit
      sub = false; // donc on ne soustrait rien
    } else if (max < _index) { // si la longueur du modulo est inférieur à la longueur du chiffre, modulo = le plus petit
      minus.set(value); // voir début de la fonction
      max = minus._index; // pareil
      uint64_t offset = _index - max; // l'offet pour le multiplicateur du modulo, si on a 43001 % 10 ==> 3, car 10 * (10^3) = 10000
      if(*(bigValue+index()) - '0' < minus.bigValue[minus.index()] - '0') offset -= 1; // evite le cas ou 43001 % 50 ==> 43001 % 50000 et transforme 50000 en 5000;
      minus.loffset(offset); //genère un nouveau minus par rapport à l'offset, si 43000 % 10, avec 10 = modulo, alors on a 43001 % 10000, pour réduire le nb de soustractions
      //minus.show();
      subtract(minus.get()); // donc on soustrait
    } else { // longueur indentique on doit donc trouver qui est le plus grand entre les deux
       if (fast_compare(bigValue+index(),value,MAX) >= 0) {
           subtract(value);
       } else {
           sub = false;
       }
    }
  }
  
} // todo à améliorer, fuite mémoire ici !!!!!!!!!!!!!!!!!

void Currency::loffset(const int value) {
  memcpy(bigValue+index()-value, bigValue+index(), _index);
  memset(bigValue+index(),'0',value);
  memset(bigValue+MAX,'\0',1); // wtf pas normal
  _index += value;
} // good, mais pas parfait

void Currency::show() const {
  printf("Current %.*s\n",_index,bigValue+index());
} // good

void Currency::reset() {
  memset(bigValue,'0',MAX);
  _index = 1;
} // good

char* Currency::get() const {
  return bigValue+index();
} // good

bool Currency::isNotPrime() const {
  return (*(bigValue+MAX-1) == '0' || *(bigValue+MAX-1) == '2' || *(bigValue+MAX-1) == '4' || *(bigValue+MAX-1) == '5' || *(bigValue+MAX-1) == '6' || *(bigValue+MAX-1) == '8');
} // good

void Currency::set(const char* value) {
  reset(); // init du tableau à '0'
  _index = strlen(value); // longueur de la valeur à copier
  memcpy(bigValue+MAX-_index,value,_index); // copie de la valeur 
} // good

uint64_t Currency::index() const {
  return MAX-_index;
} // good

//---------------------------------------------------------

Currency Currency::operator<<(int v1) const {
  Currency a(this->get());
  a.loffset(v1);
  return a;
} // good

Currency Currency::operator++(int v1) {
  this->add("1");
  return *this;
} // good

Currency Currency::operator+=(Currency& v1) {
  this->add(v1.get());
  return *this;
} // good

Currency Currency::operator+(Currency& v1) const {
  Currency a(this->get());
  a.add(v1.get());
  return a;
} // good

Currency Currency::operator+=(const char* value) {
  this->add(value);
  return *this;
} // good

Currency Currency::operator+(const char* value) const {
  Currency a(this->get());
  a.add(value);
  return a;
} // good

Currency Currency::operator*=(Currency& v1) {
  this->multiply(v1.get());
  return *this;
} // not working

Currency Currency::operator*(Currency& v1) const {
  Currency a(this->get());
  a.multiply(v1.get());
  return a;
} // good

Currency Currency::operator*=(const char* value) {
  this->multiply(value);
  return *this;
} // good

Currency Currency::operator*(const char* value) const {
  Currency a(this->get());
  a.multiply(value);
  return a;
} // good

Currency Currency::operator-=(Currency& v1) {
  this->subtract(v1.get());
  return *this;
} // good

Currency Currency::operator-(Currency& v1) const {
  Currency a(this->get());
  a.subtract(v1.get());
  return a;
} // good

Currency Currency::operator-=(const char* value) {
  this->subtract(value);
  return *this;
} // good

Currency Currency::operator-(const char* value) const {
  Currency a(this->get());
  a.subtract(value);
  return a;
} // good

Currency Currency::operator%(Currency& v1) const {
  Currency a(this->get());
  a.modulo(v1.get());
  return a;
} // good

/**
* \brief Indique si le comparé est inférieur ou égale au comparant
* \param v1 Le chiffre comparant
* \return true si comparé inférieur ou égale au comparant
* \return false si comparé supérieur au comparant
*/
bool Currency::operator<=(Currency& v1) const {
  if(this->_index > v1._index) return false; // si la longueur du chiffre de gauche est inférieure à a longueur du chiffre de droite, gauche = le plus petit
  if (this->_index < v1._index) return true; // si la longueur du chiffre de gauche est supérieure à a longueur du chiffre de droite, droite = le plus petit
  return fast_compare(this->bigValue + this->index(), v1.bigValue + v1.index(), MAX) <= 0; // longueur indentique on doit donc trouver qui est le plus grand entre les deux
} // good

/**
* \brief Indique si le comparé est supérieur ou égale au comparant
* \param v1 Le chiffre comparant
* \return true si comparé supérieur ou égale au comparant
* \return false si comparé inférieur au comparant
*/
bool Currency::operator>=(Currency& v1) const {
  if(this->_index > v1._index) return true;// si la longueur du chiffre de gauche est supérieure à a longueur du chiffre de droite, droite = le plus petit
  if (v1._index < this->_index) return false; // si la longueur du chiffre de gauche est inférieur à a longueur du chiffre de droite, gauche = le plus petit
  return fast_compare(this->bigValue + this->index(), v1.bigValue + v1.index(), MAX) >= 0;// longueur indentique on doit donc trouver qui est le plus grand entre les deux
} // good

/**
* \brief N'est pas un vrai égale, on ne cherche qu'à savoir si c'est égal à 0
* \param v1 Le chiffre comparant
* \return true si égale à 0
* \return false si différent de 0
*/
bool Currency::operator==(Currency& v1) const {
  return (v1._index == 1 && this->_index == 1 && v1.bigValue[MAX-1] == '0' && this->bigValue[MAX-1] == '0');
} // good

//---------------------------------------------------------

int main() {
  #if DEBUG
  const clock_t begin = clock();
  #endif
  
  uint64_t k = 100; // le X ème nombre premier que l'on cherche (ra)
  uint64_t it = 0; // d'ittérations 
  Currency i = "42953"; // ne peux pas être < 2 sinon boucle infinie, peut être remplacé par un précédent nombre premier
  Currency j = "0";
  Currency zero = "0";
  Currency a = "65842", b = "65845";
  
  
  while(k) {
    bool isPrime=true;
    for(j="3"; j*j<=i; j += "2"){
      if(i%j==zero) {
          isPrime=false;
          break;
      }
      it++; // itérations
    }
    if(isPrime) {
      k--;
    }
    i += "2";
  }
  
  i -= "2";
  
  #if DEBUG
  const clock_t end = clock();
  
  i.show();
  printf("Itérations : %lld\n",it);
  printf("Duration = %f s\n",double(end - begin) / CLOCKS_PER_SEC);
  #endif
}
