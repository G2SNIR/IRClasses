#include <iostream>

/// @brief La classe IRConversion contient des fonctions qui facilitent le développement.
class IRConversion {
    public:
        static int asciiToHex(std::string texte, unsigned char * hexa, unsigned int taille);
        static int hexToAscii( unsigned const char * hexa, unsigned int taille, std::string & texte);

};