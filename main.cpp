#include <iostream>
#include <vector>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
    string i ="0";
    cout << "================ Menu :" <<endl
         << "================ [1]: Entrer un nom de fichier manuellement" << endl
         << "================ [2]: Lancer les programmes test de la Demo" << endl
            << "================ [3]: Lancer les programmes test complets hors demo" << endl;
    getline(cin, i);
    if (i == "1"){
            string nomFich;
            if (argc != 2) {
                cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
                cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
                getline(cin, nomFich);
            } else
                nomFich = argv[1];
            ifstream fichier(nomFich.c_str());
            try {
                if (fichier.fail()) throw FichierException();
                Interpreteur interpreteur(fichier);
                interpreteur.analyse();
                if (interpreteur.getArbre() != nullptr){
                    // Si pas d'exception levée, l'analyse syntaxique a réussi
                    cout << endl << "================ Syntaxe Correcte" << endl;
                    // On affiche le contenu de la table des symboles avant d'exécuter le programme
                    cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
                    cout << endl << "================ Execution de l'arbre" << endl;
                    // On exécute le programme si l'arbre n'est pas vide
                    if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
                    // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
                    cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
                    FILE *fp = fopen("tradCPP.txt", "w");
                    fclose(fp); 
                    string fichCPP;
                    fichCPP = "tradCPP.txt";
                    fstream fichierCPP(fichCPP.c_str());
                    interpreteur.traduitEnCPP(fichierCPP,0);
                }

            } catch (InterpreteurException & e) {
                cout << e.what() << endl;
            }
    
        }
    else if(i=="2"){
        
        string nomFich;
        string fichiers[10] = { "testDemo01-SiSinonSi_1.txt", "testDemo02-tantQue_1.txt", "testDemo03-Repeter_1.txt", "testDemo04-Pour_1.txt", "testDemo05-Ecrire_1.txt", "testDemo07-Exception_1.txt", "testDemo08-Priorité_1.txt", "testDemo09-Selon_1.txt", "testDemo10-Incrémentation_1.txt", "testDemo06-Lire_1.txt"};
        
        for(int i =0;i<10;i++){
            cout << "======================================================================================================================================" <<endl
                 << "======================================================================================================================================"<<endl;
            cout << "===============Fichier : " << fichiers[i].c_str();
            FILE *fp = fopen("testDemo-TradCPP.txt", "w");
            fclose(fp); 
            ifstream fichier(fichiers[i].c_str());
            try {
                    if (fichier.fail()) throw FichierException();
                    Interpreteur interpreteur(fichier);
                    interpreteur.analyse();
                    // Si pas d'exception levée, l'analyse syntaxique a réussi
                    cout << endl << "================ Syntaxe Correcte" << endl;
                    // On affiche le contenu de la table des symboles avant d'exécuter le programme
                    cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
                    cout << endl << "================ Execution de l'arbre" << endl;
                    // On exécute le programme si l'arbre n'est pas vide
                    if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
                    // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
                    cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();

                    string fichCPP;
                    fichCPP = "testDemo-TradCPP.txt";
                    fstream fichierCPP(fichCPP.c_str());
                    interpreteur.traduitEnCPP(fichierCPP,0);

                } catch (InterpreteurException & e) {
                    cout << e.what() << endl;
                }
            cout << "========Appuyez sur Entrée pour passer au test suivant."<<endl;
            getchar();
        }
    }
    else if(i=="3"){
        
        string nomFich;
        string fichiers[10] = { "test01-SiSinonSi.txt", "test02-tantQue.txt", "test03-Repeter.txt", "test04-Pour.txt", "test05-Ecrire.txt", "test07-Exception.txt", "test08-Priorité.txt", "test09-Selon.txt", "test10-Incrémentation.txt", "test06-Lire.txt"};
        
        for(int i =0;i<10;i++){
            cout << "======================================================================================================================================" <<endl
                 << "======================================================================================================================================"<<endl;
            cout << "===============Fichier : " << fichiers[i].c_str();
            FILE *fp = fopen("testDemo-TradCPP.txt", "w");
            fclose(fp); 
            ifstream fichier(fichiers[i].c_str());
            try {
                    if (fichier.fail()) throw FichierException();
                    Interpreteur interpreteur(fichier);
                    interpreteur.analyse();
                    // Si pas d'exception levée, l'analyse syntaxique a réussi
                    cout << endl << "================ Syntaxe Correcte" << endl;
                    // On affiche le contenu de la table des symboles avant d'exécuter le programme
                    cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
                    cout << endl << "================ Execution de l'arbre" << endl;
                    // On exécute le programme si l'arbre n'est pas vide
                    if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
                    // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
                    cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();

                    string fichCPP;
                    fichCPP = "tradCPP.txt";
                    fstream fichierCPP(fichCPP.c_str());
                    interpreteur.traduitEnCPP(fichierCPP,0);

                } catch (InterpreteurException & e) {
                    cout << e.what() << endl;
                }
            cout << "========Appuyez sur Entrée pour passer au test suivant."<<endl;
            getchar();
        }
    }
    return 0;
}
