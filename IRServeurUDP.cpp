#include "IRServeurUDP.h"

#include <iostream>
#include <string.h> // memset


int IRServeurUDP::m_nbServeurUDP = 0;

 IRServeurUDP::IRServeurUDP()
 {
    m_adresseIPDEcoute = "127.0.0.1";
    m_portDEcoute = 4000;
    if(m_nbServeurUDP == 0)
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
	m_nbServeurUDP++;

}

IRServeurUDP::~IRServeurUDP()
{
    FermerLaSocket();
}


 bool IRServeurUDP::OuvrirLaSocketDEcoute( unsigned short portDEcoute, std::string adresseIPDEcoute)
 {
    m_portDEcoute = portDEcoute;
    m_adresseIPDEcoute = adresseIPDEcoute;
    m_socketServeurUDP = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( m_socketServeurUDP == -1 )
    {
        std::cout << "Erreur de création de socket (socket())." << std::endl;
        return false;
    }

    m_addrServeur.sin_family = AF_INET;
    m_addrServeur.sin_port = htons ( m_portDEcoute );
    m_addrServeur.sin_addr.s_addr = inet_addr(m_adresseIPDEcoute.c_str());

    if( bind(m_socketServeurUDP, (const sockaddr *)&m_addrServeur, sizeof(m_addrServeur)) != 0)
    {
        std::cout << "Erreur lors de l'association de l'adresse IP et du port à la socket (bind())." << std::endl;
        return false;
    }

    return true;
}

bool IRServeurUDP::FermerLaSocket()
{
        #ifdef __unix__
    close(m_socketServeurUDP);
    #elif defined(_WIN32) || defined(_WIN64)
    closesocket(m_socketServeurUDP);
    #endif

    return true;

}

bool IRServeurUDP::EnvoyerUnMessage(std::string message)
{
    int nbCaractEmi;
    nbCaractEmi = sendto ( m_socketServeurUDP, message.c_str(), message.length()+1 , 0,  &from, sizeof(from));
    // On vérifie qu'un message a bien été reçu
    if(nbCaractEmi == -1)
    {
        return false;
    }
    return true;
}

bool IRServeurUDP::EnvoyerDesOctets(const unsigned char * tableau, int taille)
{
    int nbCaractEmi;
    nbCaractEmi = sendto ( m_socketServeurUDP, (const char*)tableau, taille , 0,  &from, sizeof(from));
    // On vérifie qu'un message a bien été reçu
    if(nbCaractEmi == -1)
    {
        return false;
    }
    return true;
}

int IRServeurUDP::RecevoirUnMessage(std::string & message)
{
    char msg[1500];
    int length = sizeof(from); // Doit être initialisé
     // Préconisation de la documentation : Mise à 0 de la structure from avant le recvfrom
    memset(&from, 0, sizeof(from));
   int nbCaracteresRecus = recvfrom(m_socketServeurUDP, msg, 1500, 0, (sockaddr *)&from, (socklen_t *)&length);
    std::cout << "RECV : " << nbCaracteresRecus << std::endl;  
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

int IRServeurUDP::RecevoirDesOctets(unsigned char * tableau, int tailleMax)
{
    int length = sizeof(from);
    // Préconisation de la documentation : Mise à 0 de la structure from avant le recvfrom
    memset(&from, 0, sizeof(from));
    int nbCaracteresRecus = recvfrom(m_socketServeurUDP, tableau, tailleMax, 0, (sockaddr *)&from, (socklen_t *)&length);
    if(nbCaracteresRecus > 1)
    {
        return nbCaracteresRecus;
    }
    return -1;
}
