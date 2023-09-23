#include "IRConversion.h"



// Cette fonction renvoi la longueur du tableau hexa.
// Elle renvoie -1 si la taille du tableau hexa n'est pas suffisante
// Elle renvoie -2 si un caractère n'est pas de l'hexa
int IRConversion::asciiToHex(std::string texte, unsigned char * hexa, unsigned int taille)
{
    if(taille < texte.length()/2) return -1;
    for(unsigned int i=0 ; i<taille ; i++ ) hexa[i] = 0;
    for(unsigned int i=0 ; i<texte.length() ; i++ )
    {
        unsigned char quartet = 0;
        if(texte[i] >= '0' && texte[i] <='9') quartet = texte[i] - '0';
        else if(texte[i] >= 'A' && texte[i] <='Z') quartet = texte[i] - 'A' + 10;
        else if(texte[i] >= 'a' && texte[i] <='z') quartet = texte[i] - 'a' + 10;
        else return -2;
        //std::cout << "quartet " << i << " : " << std::hex << (int)quartet << " " ;
        hexa[i/2] = hexa[i/2] | quartet << (4*((i+1)%2));
    }
    return texte.length()/2;
}


int IRConversion::hexToAscii( const unsigned char * hexa, unsigned int taille, std::string & texte)
{
    texte = "";
    for(unsigned int i=0 ; i<taille ; i++ )
    {
        for(int j=0 ; j<=1 ; j++)
        {   unsigned char octet = hexa[i];
            if(j == 0) octet = octet >> 4;
            if(j == 1) octet = octet & 0x0F;
            if(octet>=0 && octet<=9 ) texte += (char)(octet + '0');
            else texte += (octet - 10 + 'A');
        }
    }
    return texte.length();
}