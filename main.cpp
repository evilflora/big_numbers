#include <iostream>
#include <string.h>

#define MAX 1000
#define DEBUG 1

typedef unsigned long long bigInt;

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
    
    public :
    Currency();
    void add(const char*);
    void subtract(const char*);
    void multiply(const char*);
    void reset();
    void show();
    
    private :
    bigInt index(); /* retourne l'index du premier chiffre != 0 en partant de la gauche */
    bool verify_content(const char* ); /* vérifie que la chaine d'entrée est composée de chiffres */
};

Currency::Currency() {
    reset();
}

void Currency::add(const char* value) {
    if(verify_content(value)) {
        bigInt max = strlen(value);
        bigInt i = 0;
        short offset = 0;
        short tmp = 0;
        bool stop = false;
        short a,b;
        while (!stop) { // tant que stop est à false
            a = *(bigValue+MAX-i-1) - '0';
            if (offset && i == max) { // si on à un offset mais qu'on a parcouru tout le chiffre à additionner 
                b = 0;
                max++; // on augmente la taille pour continuer à parcourir le chiffre de base, car il reste l'offset à additionner
            } else {
                b = *(value+max-i-1) - '0';
            }
            tmp = a + b + offset; // on aditionne les deux chiffres et l'offset
            *(bigValue+MAX-i-1) = (tmp % 10) + '0'; // avec une addition entre 2 chiffres si le chiffre est supérieur à 10 on ne regarde que reste
            offset = tmp / 10; // calcul de l'offset, = 1 si >= 10 et =0 < 10
            i++;  // on augmente l'index
            if (!offset && i == max) stop = true; // s'il n'y a pas d'offset et qu'on a parcouru tour le nombre à additionner alors on quitte
        }
    }
}

void Currency::subtract(const char* value) {
    if(verify_content(value)) {
        bigInt max = strlen(value);
        bigInt i = 0;
        short offset = 0;
        short tmp = 0;
        bool stop = false;
        short a,b;
        while (!stop) { // tant que stop est à false
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
            if (!offset && i == max) stop = true; // s'il n'y a pas d'offset et qu'on a parcouru tour le nombre à soustraire alors on quitte
        }
    }
}

void Currency::show() {
    printf("Current %.*s\n",MAX-index(),bigValue+index());
}

void Currency::reset() {
    memset(bigValue,'0',MAX);
}

void Currency::multiply(const char* value) {
    if(verify_content(value)) {
        bigInt max = strlen(value);
        bigInt max_bigvalue = MAX - index(); // longueur du big value, pour savoir le nombre de case à multiplier
        char local_value[MAX];
        bigInt i = 0,j = 0;
        short offset = 0;
        short tmp = 0;
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
    }
}

bool Currency::verify_content(const char* value) { // todo : désactivé, temporaire
    /*bigInt max = strlen(value);
    bigInt i;
    for(i = 0; i < max; i++) {
        if (value[i] < '0' || value[i] > '9') return false;
    }*/
    return true;
}

bigInt Currency::index(){
    bigInt i = 0;
    for(i = 0; i < MAX; i++) {
        if (bigValue[i] != '0') return i;
    }
    return MAX-1;
}

int main() {
    Currency current;

    #if DEBUG
    clock_t begin = clock();
    #endif
    
	current.add("15783");
	current.show();
	current.add("1578350");
	current.show();
	current.multiply("8232");
	current.show();
	
    #if DEBUG
    clock_t end = clock();
    printf("Duration = %f s\n",(double)(end - begin) / CLOCKS_PER_SEC);
    #endif
}
