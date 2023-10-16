//---------------------------------------------------------------------------

#ifndef IRJSONH
#define IRJSONH

#include <string>
#include <vector>

#define TYPE_CHAINE 0
#define TYPE_ENTIER 1
#define TYPE_JSONOBJECT 2
#define TYPE_TABLEAU 3
#define TYPE_TABLEAU_JSONOBJECT 4
#define TYPE_TABLEAU_VALEUR 5
#define TYPE_ERREUR 6

//---------------------------------------------------------------------------

/*JSON Syntax Rules
Data is in name/value pairs
Data is separated by commas
Curly braces hold objects
Square brackets hold arrays
*/
class IRJSONData;

class IRJSONObject {
    private:
        std::vector <IRJSONData *>  donnees;
        unsigned int niveau;
    public:
        IRJSONObject(std::string json_text = "");
        ~IRJSONObject();
        unsigned int Extraction(std::string json_text, unsigned int position = 0, int niveau = 0);
        std::string Description(/*unsigned int niveau = 0*/);
        inline unsigned int NombreDeDonnees() {return donnees.size();};
        std::string NomAttribut(unsigned int indice);
        int Indice(std::string attribut) const;
        std::string Valeur(std::string attribut) const;
        const IRJSONObject * Objet(std::string attribut) const;
        const IRJSONData * Data(unsigned int indice); 
        const IRJSONData * operator[](unsigned int indice);
        inline unsigned int Niveau() const {return niveau;};

};

class IRJSONData {
    private:
        int type;  
        std::string attribut; 
        std::string valeur;
        IRJSONObject * fils;
        std::vector <IRJSONObject *> tab;
        const IRJSONObject * pere;

    public:
        IRJSONData(const IRJSONObject * monPere);
        ~IRJSONData();
        unsigned int Extraction(std::string json_text, unsigned int positionDepart=0);
        std::string Description(/*unsigned int niveau = 0*/);
        inline std::string Valeur() const {if(type == TYPE_CHAINE || type == TYPE_ENTIER) return valeur; else return "";};
        inline std::string Attribut() const {return attribut;};
        inline int Type() const {return type;};
        inline const IRJSONObject * Fils () const {return fils;};
        inline const std::vector <IRJSONObject *> & Tab() const {return tab;};        
};


#endif
