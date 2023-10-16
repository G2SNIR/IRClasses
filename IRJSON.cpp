//---------------------------------------------------------------------------

#include "IRJSON.h"
#include <sstream>
#include <iostream> 

//#define DEBUG

const IRJSONObject IRJSONObject_null;
const IRJSONData IRJSONData_null(&IRJSONObject_null);

IRJSONObject::IRJSONObject(std::string json_text)
{
    //Extraction(json_text);
}

IRJSONObject::~IRJSONObject()
{
    // On parcours le tableau et on supprime tous les pointeurs
    for(unsigned int i=0; i<donnees.size() ; i++)
    {
        delete donnees[i];
        donnees[i] = NULL;
    }
    donnees.clear();
    #ifdef DEBUG 
    std::cout << "*** DEBUG - Suppression des elements.\n"; 
    #endif
}

unsigned int IRJSONObject::Extraction(std::string json_text, unsigned int position, int niveau)
{
    // On récupère le niveau
    this->niveau = niveau;
    // Si le niveau est 0, on enlève du texte tous les caractères génants (\n, \r, \t)
    #ifdef DEBUG 
    std::cout << "*** DEBUG - Texte avant : \n";
	std::cout << json_text << '\n'; 
    #endif
	// Préparation du fichier : suppression des sauts de ligne et des tabulations
	unsigned int j = 0;
	while(j < json_text.length())
	{
		if(json_text[j] == '\n' || json_text[j] == '\r' || json_text[j] == '\t') {json_text.erase(j,1);}
		else {j++;}
	}
    #ifdef DEBUG 
	std::cout << "*** DEBUG - Texte apres : \n";
    #endif
    // On vérifie que le texte commence par une accolade
    unsigned int i = position;
    while(json_text[i] == ' ' && i<json_text.length()) {i++;}
    //unsigned int premierCaractere = i;
    if(i == json_text.length()) {
        #ifdef DEBUG 
        std::cout << "*** DEBUG - La chaine ne contient que des caractères espaces." << std::endl;
        #endif
        return false;
    }
    if(json_text[i] != '{') {
        #ifdef DEBUG
        std::cout << "*** DEBUG - Le caractère '{' n'a pas ete trouve." << std::endl;
        #endif
        return 0; 
    }
    #ifdef DEBUG
    std::cout << "**** DEBUG - Accolade ouvrante trouvee.\n"; 
    #endif
    // i contient la position de l'accolade ouvrante
    // Extraction des données
    do
    {
        IRJSONData * data = new IRJSONData(this);
        unsigned int position = data->Extraction(json_text, i+1);
        // Si la position retournée vaut 0 -> erreur
        #ifdef DEBUG
        std::cout << data->Description();
        std::cout << "*** DEBUG - position : " << position << "\n";
        #endif
        if(position == 0) {return 0;}
        donnees.push_back(data);
        // On avance jusqu'au prochain caractère
        while(json_text[position] == ' ') {position++;}
        if(json_text[position] == '}') {return position+1;} // fin extraction si accolade fermante
        if(json_text[position] != ',') {return 0;} // Erreur d'extraction si le caractère n'est pas une virgule
        i = position; // i contient la position de la virgule
    }while(i<json_text.length());
    #ifdef DEBUG
    std::cout << "*** DEBUG - On a atteint la fin de la chaine de caractères sans avoir trouvé de '}' ni de ','!!!\n";
    #endif
    return 0;   
} 

std::string IRJSONObject::Description(/*unsigned int niveau*/)
{
    std::string description = "";
    for(unsigned int i=0 ; i<donnees.size() ; i++)
    {
        description += donnees[i]->Description();
    }
    return description;
}

std::string IRJSONObject::NomAttribut(unsigned int indice) 
{
    if(indice >=0 && indice <donnees.size())
    {
        return donnees[indice]->Attribut();
    }
    return "";
}

int IRJSONObject::Indice(std::string attribut) const
{
    for(unsigned int i=0 ; i<donnees.size() ; i++)
    {
        if(donnees[i]->Attribut() == attribut)
        {
            return i;
        }
    }
    return -1;
}

std::string IRJSONObject::Valeur(std::string attribut) const
{
    int indice = this->Indice(attribut);
    if(indice == -1) return "";
    return donnees[indice]->Valeur();
}

const IRJSONObject * IRJSONObject::Objet(std::string attribut) const
{
    int indice = this->Indice(attribut);
    if(indice == -1) return &IRJSONObject_null;
    return donnees[indice]->Fils();
}



const IRJSONData * IRJSONObject::Data(unsigned int indice)
{
    if(indice >=0 && indice <donnees.size())
    {
        return donnees[indice];
    }
    return &IRJSONData_null;
}

const IRJSONData * IRJSONObject::operator[](unsigned int indice)
{
    if(indice >=0 && indice <donnees.size())
    {
        return donnees[indice];
    }
    return &IRJSONData_null;
}





IRJSONData::IRJSONData(const IRJSONObject * monPere) : pere(monPere)
{
    attribut = "";
    valeur = "";
    type = TYPE_ERREUR;
    fils = NULL;
}

IRJSONData::~IRJSONData()
{
    if(fils != NULL)
    {
        delete fils;
        fils = NULL;
    }
    for(unsigned int i=0 ; i<tab.size() ; i++)
    {
        delete tab[i];
        tab[i] = NULL;
    }
}

unsigned int IRJSONData::Extraction(std::string json_text, unsigned int positionDepart)
{
    #ifdef DEBUG
    std::cout << "*** DEBUG - positionDepart : " << positionDepart << std::endl; 
    std::cout << "*** DEBUG - json_text[positionDepart] : " << json_text[positionDepart] << std::endl; 
    #endif
    // Extraction de l'attribut
    unsigned int debutAttribut = json_text.find('"', positionDepart) + 1;
    #ifdef DEBUG
    std::cout << "*** DEBUG - debutAttribut : " << debutAttribut << std::endl; 
    #endif
    unsigned int finAttribut = json_text.find('"', debutAttribut);
    #ifdef DEBUG
    std::cout << "*** DEBUG - finAttribut : " << finAttribut << std::endl; 
    #endif
    if(!(debutAttribut == std::string::npos || finAttribut == std::string::npos))
    {
        attribut = json_text.substr(debutAttribut, finAttribut-debutAttribut);
    }
    else
    {
        attribut = "";
        type = TYPE_ERREUR;
        return 0;
    }
    // On cherche les :
    unsigned int deuxpoints = json_text.find(':', finAttribut);
    if(deuxpoints == std::string::npos)
    {
        type = TYPE_ERREUR;
        return 0;
    }
    unsigned int i = deuxpoints+1;
    while(json_text[i] == ' ') {i++;}
    #ifdef DEBUG
    std::cout << "***DEBUG - i : " << i << std::endl; 
    std::cout << "***DEBUG - json_texte[i] : " << json_text[i] << std::endl; 
    #endif
    switch (json_text[i])
    {
        case '{' :
            type = TYPE_JSONOBJECT;
            break;
        case '[' :
            type = TYPE_TABLEAU;
            break;
        case '"' :
            type = TYPE_CHAINE;
            break;
        default :
            type = TYPE_ENTIER; 
    }
    if(type == TYPE_CHAINE )
    {
        unsigned int debutValeur = i + 1;
        unsigned int finValeur = json_text.find('"', debutValeur);
        if(finValeur == std::string::npos)
        {
            type = TYPE_ERREUR;
            return 0;
        }
        else
        {
            valeur = json_text.substr(debutValeur, finValeur-debutValeur);
            return finValeur+1;
        }
    }
    else if(type == TYPE_ENTIER)  // ENTIER, REEL, BOOLEAN (true/false)
    {
        unsigned int debutValeur = i;
        unsigned int finValeur = i;
        while(json_text[finValeur] != ' ' && json_text[finValeur] != ',' && json_text[finValeur] != '}' && finValeur<json_text.length() ) {finValeur++;}
        if(finValeur == json_text.length()) 
        {
            type = TYPE_ERREUR;
            return 0;
        }
        else
        {
            valeur = json_text.substr(debutValeur, finValeur-debutValeur);
            return finValeur;
        }
    }
    else if(type == TYPE_JSONOBJECT )
    {
        #ifdef DEBUG
        std::cout << "***** NOUVEAU FILS - pere : " <<  pere->Niveau() << "\n";
        #endif
        fils = new IRJSONObject();
        int new_position = fils->Extraction(json_text, i, pere->Niveau()+1);
        #ifdef DEBUG
        std::cout << "***** NIVEAU DU FILS : " << fils->Niveau() << "\n";
        #endif
        if(new_position == 0) {return 0;}
        return new_position;
    }
    else if(type == TYPE_TABLEAU)
    {
        // On parcours jusqu'au prochain caractère. Si c'est une accolade, on est dans un tableau de JSONOBJECT
        #ifdef DEBUG
        std::cout << "*** TEST - TABLEAU - " << i << "\n";
        #endif
        unsigned int prochain_caractere = i + 1;
        while(json_text[prochain_caractere] == ' '){prochain_caractere++;}
        #ifdef DEBUG
        std::cout << "*** TEST - Prochain caractere - " << json_text[prochain_caractere] << "\n";
        #endif
        if(json_text[prochain_caractere] == '{')
        {
            do
            {
                type = TYPE_TABLEAU_JSONOBJECT;
                IRJSONObject * json_object = new IRJSONObject();
                #ifdef DEBUG
                std::cout << "*** TEST - TABLEAU JSONOBJECT - " << prochain_caractere << "\n";
                #endif
                unsigned int position = json_object->Extraction(json_text, prochain_caractere, pere->Niveau()+1);
                // Si la position retournée vaut 0 -> erreur
                if(position == 0) {return 0;}
                tab.push_back(json_object);  
                prochain_caractere = position;
                while(json_text[prochain_caractere] == ' '){prochain_caractere++;}
                if(json_text[prochain_caractere] == ']') {return prochain_caractere+1;}
                else if(json_text[prochain_caractere] != ',') {return 0;}
                #ifdef DEBUG
                std::cout << "*** TEST - Virgule trouvee : " << prochain_caractere << std::endl;  
                #endif
                prochain_caractere += 1; // On passe la virgule
            } while (prochain_caractere < json_text.length());
        }
        // SINON c'est un tableau de valeurs
    }
    return 0;
}

std::string IRJSONData::Description(/*unsigned int niveau*/)
{
    #ifdef DEBUG
    std::cout << "*** DEBUG - niveau : " << pere->Niveau() << "\n";
    std::cout << "*** DEBUG - attribut : " << attribut << "\n";
    std::cout << "*** DEBUG - valeur : " << valeur << "\n";
    #endif
    std::stringstream description;
    if(type == TYPE_CHAINE || type == TYPE_ENTIER)
    {
        for(unsigned int i=0 ; i<pere->Niveau() ; i++) description << "\t";
        description << attribut << " : " << valeur << std::endl;
    }
    else if(type == TYPE_JSONOBJECT)
    {
        for(unsigned int i=0 ; i<pere->Niveau() ; i++) description << "\t";
        description << attribut << " : " << std::endl;
        description << fils->Description() << std::endl; 
    }
    else if(type == TYPE_TABLEAU_JSONOBJECT)
    {
        for(unsigned int i=0 ; i<pere->Niveau() ; i++) description << "\t";
        description  << attribut << " : " << std::endl;
        for(unsigned j=0 ; j<tab.size() ; j++)
        {
            for(unsigned int i=0 ; i<pere->Niveau()+1 ; i++) description << "\t";
            description  << "[" << j << "]" << std::endl;
            description << tab[j]->Description() ; 
        }
    }
    return description.str();
}
