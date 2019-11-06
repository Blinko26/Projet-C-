#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include "Lecteur.h"
#include "Interpreteur.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() { // Création d'un NoeudSeqInst avec un vecteur d'instructions
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction); // ajoute une instruction au vecteur 
}

void NoeudSeqInst::traduitEnCPP(ostream & cout,unsigned int indentation) const{
    for (unsigned int i = 0; i < m_instructions.size(); i++){
        m_instructions[i]->traduitEnCPP(cout,indentation); // traduit chaque instruction de la séquence
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) { // Création d'un NoeudAffectation avec une variable et une expression
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout,unsigned int indentation) const{ 
//Traduit l'instruction en CPP
    cout<<setw(4*(indentation))<<""<< ((SymboleValue*)m_variable)->getChaine() << " = "; // Ecrit le nom de la variable = avec 4*indentation espaces
    m_expression->traduitEnCPP(cout,0); // Traduit l'expression en CPP
    
    cout << ";" << endl; // Ecrit ";" puis revient à la ligne
    
}

void NoeudAffectation::traduitEnCPPPour(ostream & cout,unsigned int indentation) const{ 
//Traduit l'instruction en CPP
    cout<<setw(4*(indentation))<<""<< ((SymboleValue*)m_variable)->getChaine() << " = "; // Ecrit le nom de la variable = avec 4*indentation espaces
    m_expression->traduitEnCPP(cout,0); // Traduit l'expression en CPP
    
}
////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) { // Création d'un NoeudOperateurBinaire avec deux opérandes et un opérateur
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException(); // On retourne une erreur si l'opérande droite est égale à 0
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout,unsigned int indentation) const{
    
    m_operandeGauche->traduitEnCPP(cout,0); // On traduit l'opérande gauche en CPP
    if(m_operateur == "ou" ){ // On traduit chaque opérateur en CPP
        cout << "||";
    }
    else if (m_operateur == "et"){
        cout << "&&";
    }
    else {
        cout << m_operateur.getChaine();
    }
    m_operandeDroit->traduitEnCPP(cout,0); // On traduit l'opérande droite en CPP
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) { // Création d'un NoeudInstSi avec une condition et une séquence
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer(); // On vérifie la condition puis on exécute la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP(ostream & cout,unsigned int indentation) const{  
    cout << setw(4*indentation)<<""<<"if (";// Ecrit "if (" avec un décalage de 4*indentation espaces   
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(vector<Noeud *> vectInst)
:m_vectInst(vectInst) { // Création d'un NoeudInstSiRiche avec un vecteur d'instructions
}

int NoeudInstSiRiche::executer() {
    for(int i = 0;i<m_vectInst.size();i=i+2){ // Parcourt l'ensemble du vecteur 2 par 2 pour récupérer un duo condition/instruction
        if(m_vectInst[i+1]!=NULL){ // Vérifie si le duo est complet
            if(m_vectInst[i]->executer()){  //Verifie la condition
                m_vectInst[i+1]->executer(); //Execute la condition
                break;
            }
        }
        else{
            m_vectInst[i]->executer(); //Equivalent d'un sinon execute l'instruction
        }
    }
    
    return 0;
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout,unsigned int indentation) const{    

    cout << setw(4*indentation)<<""<<"if (";// Ecrit "if (" avec un décalage de 4*indentation espaces   
    for(int i = 0;i<m_vectInst.size();i=i+2){
        if(i != m_vectInst.size()-1){
            if(i!=0){
                cout << setw(4*indentation)<<""<<"else if (";// Ecrit "else if (" avec un décalage de 8*indentation espaces
            }
            m_vectInst[i]->traduitEnCPP(cout,indentation);// Traduit la condition en C++ sans décalage 
            cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne
            m_vectInst[i+1]->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
            cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
        }
        else{
            cout << setw(4*indentation)<<""<<"else {"<< endl;// Ecrit "else if (" avec un décalage de 8*indentation espaces
            m_vectInst[i]->traduitEnCPP(cout, indentation+1);
            cout << setw(4*indentation)<<""<< "}"<<endl; // Ecrit "}" avec un décalage de 4*indentation espaces
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) { // Création d'un NoeudInstTantQue avec une condition et une séquence
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) { // On vérifie la condition
        m_sequence->executer(); // On exécute la séquence
    }
    return 0;
}

void NoeudInstTantQue::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    cout << setw(4*indentation)<<""<<"while (";// Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<") {"<< endl;// Ecrit ") {" et passe à la ligne   
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) { // Création d'un NoeudInstRepeter avec une condition et une séquence d'instructions
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer(); // On exécute l'instruction tant que la condition est vérifiée
    } while (!m_condition->executer()); // On vérifie la condition
    return 0;
}

void NoeudInstRepeter::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    
    cout << setw(4*indentation)<<""<<"do {"<<endl;// Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
    cout <<setw(4*indentation)<<""<<"} while (!(";// Ecrit ") {" et passe à la ligne  
    m_condition->traduitEnCPP(cout,0);// Traduit la condition en C++ sans décalage   
    cout <<"));"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* condition, Noeud* affectation2, Noeud* sequence)
: m_affectation1(affectation1), m_condition(condition), m_affectation2(affectation2), m_sequence(sequence){ // Création d'un NoeudInstPour avec deux affectations, une condition et une séquence d'instructions
}

int NoeudInstPour::executer() {
    for(m_affectation1!=NULL?m_affectation1->executer():0;m_condition->executer();m_affectation2!=NULL?m_affectation2->executer():0){ // On vérifie la condition et les affectations (si affectation nulle, affectation = 0)
        m_sequence->executer(); // On exécute la séquence 
    }
    return 0;
}

void NoeudInstPour::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    m_affectation1!=NULL?m_affectation1->executer():0; // On execute l'affectation si initialisée, sinon = 0
    m_affectation2!=NULL?m_affectation2->executer():0; // On execute l'affectation si initialisée, sinon = 0
    cout << setw(4*indentation)<<""<<"for ("; // On écrit "for (" avec un décalage de 4*indentatiton espaces
    if(m_affectation1!=NULL){
        m_affectation1->traduitEnCPPPour(cout,0); // Si affectation1 != 0, on traduit en CPP
    }
    cout<<";"; // On écrit ";" à la suite
    m_condition->traduitEnCPP(cout,0); // On vérifie la condition
    cout <<";"; // On écrit ";" à la suite
    if(m_affectation2!=NULL){
        m_affectation2->traduitEnCPPPour(cout,0); // Si affectation2 != 0, on traduit en CPP
    }
    cout <<"){"<<endl; // Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud *> vectInst)
:m_vectInst(vectInst) { // Création d'un NoeudInstEcrire avec un vecteur d'instructions
}

int NoeudInstEcrire::executer() {
    for(auto var : m_vectInst){ // On parcourt le vecteur
        if(typeid(*var)==typeid(SymboleValue) &&  *((SymboleValue*)var)== "<CHAINE>" ){
            cout<< ((SymboleValue*)var)->getChaine(); // Si la variable entrée est une chaîne de caractère, on l'écrit
        }
        else{
            cout << var->executer(); // Sinon, on donne la valeur de la variable
        }
    }

    return 0;
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    
    cout << setw(4*indentation)<<""<<"cout"; // On écrit "cout" avec une décalage de 4*indentation espaces
    for(int i=0; i<m_vectInst.size(); i++){
        cout << "<<";
        if(typeid(*m_vectInst[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_vectInst[i])== "<CHAINE>" ){
            cout<< ((SymboleValue*)m_vectInst[i])->getChaine(); // Si la variable est une chaîne, on l'écrit 
        }
        else{
           m_vectInst[i]->traduitEnCPP(cout,indentation); // Sinon, on écrit le nom de la variable
        }

    }
    cout<<";"<<endl; // On écrit "<< endl;" à la suite
}

////////////////////////////////////////////////////////////////////////////////
// NoeudEcrireLigne
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrireLigne::NoeudInstEcrireLigne(vector<Noeud *> vectInst)
:m_vectInst(vectInst) { // Création d'un NoeudInstEcrire avec un vecteur d'instructions
}

int NoeudInstEcrireLigne::executer() {
    for(auto var : m_vectInst){ // On parcourt le vecteur
        if(typeid(*var)==typeid(SymboleValue) &&  *((SymboleValue*)var)== "<CHAINE>" ){
            cout<< ((SymboleValue*)var)->getChaine(); // Si la variable entrée est une chaîne de caractère, on l'écrit
        }
        else{
            cout << var->executer(); // Sinon, on donne la valeur de la variable
        }
    }
    cout << "\n";
    return 0;
}

void NoeudInstEcrireLigne::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    
    cout << setw(4*indentation)<<""<<"cout"; // On écrit "cout" avec une décalage de 4*indentation espaces
    for(int i=0; i<m_vectInst.size(); i++){
        cout << "<<";
        if(typeid(*m_vectInst[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_vectInst[i])== "<CHAINE>" ){
            cout<< ((SymboleValue*)m_vectInst[i])->getChaine(); // Si la variable est une chaîne, on l'écrit 
        }
        else{
           m_vectInst[i]->traduitEnCPP(cout,indentation); // Sinon, on écrit le nom de la variable
        }

    }
    cout<<"<< endl;"<<endl; // On écrit "<< endl;" à la suite
}
////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud *> vectInst)
:m_vectInst(vectInst) { // Création d'un NoeudInstLire avec un vecteur d'instructions
}

int NoeudInstLire::executer() {
    
    for(auto var : m_vectInst){ // On parcourt le vecteur
        int x;
        cin >> x; // On lit la variable
        ((SymboleValue*)var)->setValeur(x); // On affecte au vecteur la valeur de la variable entrée
    }
    
    return 0;
}

void NoeudInstLire::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    cout << setw(4*indentation)<<""<<"cin"; // On écrit "cin" avec un décalage de 4*indentation espaces
    for(auto var : m_vectInst){ // On parcourt le vecteur
        cout << ">>";  // On écrit ">>" à la suite
        var->traduitEnCPP(cout, indentation); // On traduit la variable en CPP  
    }
    cout << "; \n"; // On écrit ";" à la suite puis on revient à la ligne
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudSelon
////////////////////////////////////////////////////////////////////////////////

NoeudInstSelon::NoeudInstSelon(Noeud* variable, vector<Noeud *> vectInst, vector<Noeud *> vectCondition)
:m_variable(variable), m_vectInst(vectInst), m_vectCondition(vectCondition) { // Création d'un NoeudInstSelon avec une variable, un vecteur d'instructions et un vecteur de conditions
}

int NoeudInstSelon::executer() {
    int i=0;
    for(auto exp : m_vectCondition){ // On parcourt le vecteur de conditions
        if(exp==NULL){
            m_vectInst.at(i)->executer();
            break;
        } else if (m_variable->executer()==exp->executer()){
            m_vectInst.at(i)->executer();
            break;
        }
        i++;
    }
 
    return 0;
}

void NoeudInstSelon::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    cout << setw(4*indentation)<<""<<"switch(";
    cout<<m_variable->executer();
    cout << ") {" <<endl;    
    int i=0;
    for(auto exp : m_vectCondition){ // On parcourt le vecteur de conditions     
        if(exp==NULL){
            cout << setw(4*indentation)<<"" <<"default :" << endl;
            m_vectInst.at(i)->traduitEnCPP(cout,indentation+1);
            cout << setw(8*indentation) <<""<<"break;"<<endl;
        } else{
            cout << setw(4*indentation)<<""<< "case ";
            exp->traduitEnCPP(cout,indentation);
            cout << " : "<<endl;
            m_vectInst.at(i)->traduitEnCPP(cout, indentation+1);
            cout << setw(8*indentation)<<""<< "break;"<<endl;

        }
        i++;
    }
    
    cout << setw(4*indentation)<<""<< "} \n";
    
}
