#include "IRClientTCP.h"

#ifdef _WIN32 || _WIN64
#include <winsock2.h>
#include <sys/types.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

int IRClientTCP::nbClientTCP = 0;

IRClientTCP::IRClientTCP()
{
    if(nbClientTCP == 0)
    {
        //cout << "Bonjour" << endl;
#ifdef _WIN32 || _WIN64
        WSADATA wsaData ;
        int iResult;

        //cout << "Bonjour 2 " << endl;

        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0)
        {
            cout << "Initialisation de la DLL Winsock : ERROR." << endl;
        }
        //cout << "Initialisation de la DLL Winsock : OK." << endl;
#endif

    }
    nbClientTCP++;
}


bool IRClientTCP::SeConnecterAUnServeur(std::string adresseIPServeur, unsigned short portServeur)
{
    // Création de la socket TCP
    m_maSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_maSocket == -1)
    {
        std::cout << "Creation de la socket : :ERREUR." << std::endl;
        return false;
    }

    m_adresseIPServeur = adresseIPServeur;
    m_portServeur = portServeur;

    // Connexion au serveur
    struct sockaddr_in serveurAJoindre;
    serveurAJoindre.sin_family = AF_INET;
    serveurAJoindre.sin_addr.s_addr = inet_addr(m_adresseIPServeur.c_str());
    serveurAJoindre.sin_port = htons(m_portServeur);
    int resultat = connect(m_maSocket, (const struct sockaddr *) &serveurAJoindre, sizeof(serveurAJoindre));
    if(resultat != 0)
    {
        std::cout << "Connexion au serveur : ERREUR." << std::endl;
        return false;
    }

    return true;
}

bool IRClientTCP::SeDeconnecter()
{
#ifdef _WIN32 || _WIN64
    closesocket(m_maSocket);
#else
    close(m_maSocket);
#endif
}

bool IRClientTCP::Envoyer(const char * requete,int longueur)
{
	int resultat = send(m_maSocket, requete, longueur, 0);
    if(resultat == -1)
    {
        std::cout << "Envoi du message : ERREUR." << std::endl;
        return false;
    }
    return true;
}
bool IRClientTCP::Envoyer(std::string requete)
{
    this->Envoyer(requete.c_str(), requete.length());
}

int IRClientTCP::Recevoir(char * reponse, int tailleMax)
{
    int nbOctetsLus = recv(m_maSocket, reponse, tailleMax, 0);
    return nbOctetsLus;
}

int IRClientTCP::Recevoir(std::string & reponse, int nbOctetsAttendus)
{
    char char_reponse[nbOctetsAttendus+1];
    int nbOctetsLus = recv(m_maSocket, char_reponse, nbOctetsAttendus, 0);
    char_reponse[nbOctetsLus] = 0;
    return nbOctetsLus;
}
