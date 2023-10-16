#ifndef IRCLIENTTCP_H
#define IRCLIENTTCP_H
#include <string>


class IRClientTCP
{

public:
    IRClientTCP();
    bool SeConnecterAUnServeur(std::string adresseIPServeur, unsigned short portServeur);
    bool SeDeconnecter();
    bool Envoyer(const char * requete,int longueur);
    bool Envoyer(std::string requete);
    int Recevoir(char * reponse, int tailleMax);
    int Recevoir(std::string & reponse, int nbOctetsAttendus=1500);



private :
    static int nbClientTCP;
    int m_maSocket;
    std::string m_adresseIPServeur;
    unsigned short m_portServeur;

};

#endif 
