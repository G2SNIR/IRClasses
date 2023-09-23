#include "IRClientUDP.h"

#include <iostream>
#include <string.h> // memset

int IRClientUDP::m_nbClientUDP = 0;

 IRClientUDP::IRClientUDP()
 {
    m_adresseIPServeur = "127.0.0.1";
    m_portServeur = 4000;
    if(m_nbClientUDP == 0)
    {
		#if defined(_WIN32) || defined(_WIN64)
		WSADATA wsaData ;
		int iResult;
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(iResult != 0)
		{
			std::cout << "Echec lors de l'initialisation de la DLL Winsock" << std::endl;
		}
		#endif
    }
	m_nbClientUDP++;

}

IRClientUDP::~IRClientUDP()
{
    FermerLaSocket();
}



 bool IRClientUDP::OuvrirLaSocketDeCommunication(std::string adresseIPServeur, unsigned short portServeur)
 {
    // Coorection du BUG vu par DI
    m_adresseIPServeur = adresseIPServeur;
    m_portServeur = portServeur;

    m_socketClientUDP = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( m_socketClientUDP == -1 )
    {
        std::cout << "Erreur de création de socket." << std::endl;
        return false;
    }

    m_addrServeur.sin_family = AF_INET;
    m_addrServeur.sin_port = htons ( m_portServeur );
    m_addrServeur.sin_addr.s_addr = inet_addr(m_adresseIPServeur.c_str());

    return true;
}

bool IRClientUDP::FermerLaSocket()
{
    #ifdef __unix__
    close(m_socketClientUDP);
    #elif defined(_WIN32) || defined(_WIN64)
    closesocket(m_socketClientUDP);
    #endif

    return true;

}

bool IRClientUDP::EnvoyerUnMessage(std::string message)
{
    int nbCaractEmi;
    nbCaractEmi = sendto ( m_socketClientUDP, message.c_str(), message.length()+1 , 0, (struct sockaddr *) &m_addrServeur, sizeof(m_addrServeur));
    // On vérifie qu'un message a bien été reçu
    if(nbCaractEmi == -1)
    {
        return false;
    }
    return true;
}

bool IRClientUDP::EnvoyerDesOctets(const unsigned char * tableau, int taille)
{
    int nbCaractEmi;
    nbCaractEmi = sendto ( m_socketClientUDP, (const char*)tableau, taille , 0, (struct sockaddr *) &m_addrServeur, sizeof(m_addrServeur));
    // On vérifie qu'un message a bien été reçu
    if(nbCaractEmi == -1)
    {
        return false;
    }
    return true;
}

int IRClientUDP::RecevoirUnMessage(std::string & message)
{
    char msg[1500];
    struct sockaddr_in from;
    // Préconisation de la documentation : Mise à 0 de la structure from avant le recvfrom
    memset(&from, 0, sizeof(from));
    unsigned int length = sizeof(from); // doit être initialisé
    int nbCaracteresRecus = recvfrom(m_socketClientUDP, msg, 1500, 0, (struct sockaddr *)&from, &length);    
    if(nbCaracteresRecus > 1)
    {
        if(msg[nbCaracteresRecus-1] == 0)
            message = std::string(msg, nbCaracteresRecus-1);
        else
            message = std::string(msg, nbCaracteresRecus);
        return nbCaracteresRecus;
    }
    message = "";
    return -1;
}

int IRClientUDP::RecevoirDesOctets(unsigned char * tableau, int tailleMax)
{
    struct sockaddr_in from;
    unsigned int length = sizeof(from);
    // Préconisation de la documentation : Mise à 0 de la structure from avant le recvfrom
    memset(&from, 0, sizeof(from));
    int nbCaracteresRecus = recvfrom(m_socketClientUDP, tableau, tailleMax, 0, (struct sockaddr *)&from, &length);
    if(nbCaracteresRecus > 1)
    {
        return nbCaracteresRecus;
    }
    return -1;
}
