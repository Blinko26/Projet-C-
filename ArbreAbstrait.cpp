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

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
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
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout,unsigned int indentation) const{ 
//Traduit l'instruction en CPP
    cout<<setw(4*(indentation))<<""<< ((SymboleValue*)m_variable)->getChaine() << " = ";
    m_expression->traduitEnCPP(cout,0);
    
    cout << ";" << endl; // maVariable = monExpression;
    
}

void NoeudAffectation::traduitEnCPPPour(ostream & cout,unsigned int indentation) const{ 
//Traduit l'instruction en CPP
    cout<<setw(4*(indentation))<<""<< ((SymboleValue*)m_variable)->getChaine() << " = ";
    m_expression->traduitEnCPP(cout,0);
    
}
////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
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
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout,unsigned int indentation) const{
    
    m_operandeGauche->traduitEnCPP(cout,0);
    if(m_operateur == "ou" || m_operateur == "et"){
        if(m_operateur == "ou" ){
            cout << "||";
        }
        else if (m_operateur == "et"){
            cout << "&&";
        }
    }else {
        cout << m_operateur.getChaine();
    }
    m_operandeDroit->traduitEnCPP(cout,0);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
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
:m_vectInst(vectInst) {
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
            cout << setw(4*indentation)<<""<< "}"<<endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) {
        m_sequence->executer();
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
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer();
    } while (!m_condition->executer());
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
: m_affectation1(affectation1), m_condition(condition), m_affectation2(affectation2), m_sequence(sequence){
}

int NoeudInstPour::executer() {
    for(m_affectation1!=NULL?m_affectation1->executer():0;m_condition->executer();m_affectation2!=NULL?m_affectation2->executer():0){
        m_sequence->executer();
    }
    return 0;
}

void NoeudInstPour::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    m_affectation1!=NULL?m_affectation1->executer():0;
    m_affectation2!=NULL?m_affectation2->executer():0;
    cout << setw(4*indentation)<<""<<"for (";
    if(m_affectation1!=NULL){
        m_affectation1->traduitEnCPPPour(cout,0);
    }
    cout<<";";
    m_condition->traduitEnCPP(cout,0); 
    cout <<";";
    if(m_affectation2!=NULL){
        m_affectation2->traduitEnCPPPour(cout,0);
    }
    cout <<"){"<<endl; // Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_sequence->traduitEnCPP(cout, indentation+1);// Traduit en C++ la séquence avec indentation augmentée   
    cout << setw(4*indentation)<<""<<"}"<< endl;// Ecrit "}" avec l'indentation initiale et passe à la ligne 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud *> vectInst)
:m_vectInst(vectInst) {
}

int NoeudInstEcrire::executer() {
    for(auto var : m_vectInst){
        if(typeid(*var)==typeid(SymboleValue) &&  *((SymboleValue*)var)== "<CHAINE>" ){
            cout<< ((SymboleValue*)var)->getChaine();
        }
        else{
            cout << var->executer(); 
        }
    }
    cout << "\n";
    return 0;
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    
    cout << setw(4*indentation)<<""<<"cout";
    for(int i=0; i<m_vectInst.size(); i++){
        cout << "<<";
        if(typeid(*m_vectInst[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_vectInst[i])== "<CHAINE>" ){
            cout<< ((SymboleValue*)m_vectInst[i])->getChaine();
        }
        else{
           m_vectInst[i]->traduitEnCPP(cout,indentation); 
        }

    }
    cout<<"<< endl;"<<endl;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud *> vectInst)
:m_vectInst(vectInst) {
}

int NoeudInstLire::executer() {
    
    for(auto var : m_vectInst){
        int x;
        cin >> x;
        ((SymboleValue*)var)->setValeur(x);       
    }
    
    return 0;
}

void NoeudInstLire::traduitEnCPP(ostream & cout,unsigned int indentation) const{    
    cout << setw(4*indentation)<<""<<"cin";
    for(auto var : m_vectInst){
        cout << ">>"; 
        var->traduitEnCPP(cout, indentation);       
    }
    cout << "; \n";
    
}