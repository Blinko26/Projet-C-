#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <typeinfo>
#include <vector>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) { // Création d'un interpréteur 
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
      
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
    
    
      
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
          
  throw SyntaxeException(messageWhat);
}

void Interpreteur::traduitEnCPP(ostream & cout,unsigned int indentation)const{  
    cout << setw(4*indentation)<<""<<"int main() {"<< endl; // Début d’un programme C++
    // Ecrire en C++ la déclaration des variables présentes dans le programme... 
    // ... variables dont on retrouvera le nom en parcourant la table des symboles ! 
    // Par exemple, si le programme contient i,j,k, il faudra écrire : int i; int j; int k; ...   
    for (int i = 0;i < m_table.getTaille();i++){
        if(m_table[i]=="<VARIABLE>"){
            cout<<setw(4*(indentation+1))<<""<<"int"<<" "<<m_table[i].getChaine()<<";"<<endl; // Declare un int et l'écrit en CPP
        }
    }
    getArbre()->traduitEnCPP(cout,indentation+1);// lance l'opération traduitEnCPP sur la racine  
    cout << setw(4*(indentation+1))<<""<<"return 0;"<< endl ; // Ecrit "return 0;" puis revient à la ligne
    cout << setw(4*indentation)<<"}" << endl ; // Fin d’un programme C++
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire" || m_lecteur.getSymbole() == "selon");
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  // Retourne la procédure à utiliser en fonction du symbole courant
    
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
      Noeud *affect = affectation();
      testerEtAvancer(";");
      return affect;
    }
    else if (m_lecteur.getSymbole() == "si")
      return instSiRiche();
    else if (m_lecteur.getSymbole() == "tantque")
      return instTantQue();
    else if (m_lecteur.getSymbole() == "repeter"){
      return instRepeter();
    }
    else if (m_lecteur.getSymbole() == "pour")
      return instPour(); 
    else if (m_lecteur.getSymbole() == "ecrire")
      return instEcrire();
    else if (m_lecteur.getSymbole() == "lire")
      return instLire();
    else if (m_lecteur.getSymbole() == "selon")
      return instSelon();
    // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    else {
        erreur("Erreur: Instruction");
        return nullptr;
    }
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
  m_lecteur.avancer();
  
  //ajout pour l'incrémentation
  Symbole operateur = m_lecteur.getSymbole();
  Noeud* exp;
  if(operateur=="+" || operateur =="-" || operateur =="*" || operateur =="/"){
      m_lecteur.avancer();
      testerEtAvancer("=");
      Noeud* op = m_table.chercheAjoute(m_lecteur.getSymbole()); // L'operateur est ajouté à la table et on le mémorise
      m_lecteur.avancer();
      exp = new NoeudOperateurBinaire(operateur,var , op); // Et on construit un noeud opérateur binaire
  }
  else{
      testerEtAvancer("=");
        exp = expression();             // On mémorise l'expression trouvée
  }
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* et = expEt();
  while (m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* etDroit = expEt(); // On mémorise l'opérande droit
    et = new NoeudOperateurBinaire(operateur, et, etDroit); // Et on construit un noeud opérateur binaire
  }
  return et; // On renvoie et qui est un opérateur binaire
}

Noeud* Interpreteur::expEt() {
    // <expEt> ::= <expComp> {et <expComp> }
    
    Noeud* comp = expComp();
    while (m_lecteur.getSymbole() == "et") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole (ici "et")
        m_lecteur.avancer();
        Noeud* compDroit = expComp();
        comp = new NoeudOperateurBinaire(operateur, comp, compDroit); // Et on construit un noeud opérateur binaire
    }
    
    return comp; // On renvoie comp qui est un opérateur binaire
}

Noeud* Interpreteur::expComp() {
    // <expComp> ::= <expAdd> {==|!=|<|<=|>|>= <expAdd> }
    
    Noeud* add = expAdd();
    while (m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole
        m_lecteur.avancer();
        Noeud* addDroit = expAdd();
        add = new NoeudOperateurBinaire(operateur, add, addDroit); // Et on construit un noeud opérateur binaire
    }
    
    return add; // On renvoie add qui est un opérateur binaire
}

Noeud* Interpreteur::expAdd() {
    // <expAdd> ::= <expMult> {+|-<expMult> }
    
    Noeud* mult = expMult();
    while (m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole
        m_lecteur.avancer();
        Noeud* multdroit = expMult();
        mult = new NoeudOperateurBinaire(operateur, mult, multdroit); // Et on construit un noeud opérateur binaire
    }
    
    return mult; // On renvoie mult qui est un opérateur binaire
}

Noeud* Interpreteur::expMult() {
    // <expMult> ::= <facteur> {*|/<facteur> }
    
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"){
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole
        m_lecteur.avancer();
        Noeud* factDroit = facteur();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construit un noeud opérateur binaire
    }
    
    return fact; // On renvoie fact qui est un opérateur binaire
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur()); // Création d'un nouvel opérateur binaire
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr); // Création d'un nouvel opérateur binaire
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
  // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Si"
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instSiRiche() {
    // <instSiRiche> ::=si(<expression>) <seqInst> {sinonsi(<expression>) <seqInst> }[sinon <seqInst>]finsi
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "SiRiche"
    vector<Noeud *> v_inst;
    bool test = true; // Vrai tant que l'on a "sinonsi"
    testerEtAvancer("si");
    testerEtAvancer("(");
    v_inst.push_back(expression()); // On mémorise la condition
    testerEtAvancer(")");
    v_inst.push_back(seqInst());     // On mémorise la séquence d'instruction
    while (test) {
        if (m_lecteur.getSymbole()=="sinonsi") {
            testerEtAvancer("sinonsi");
            testerEtAvancer("(");
            v_inst.push_back(expression()); // On mémorise la condition
            testerEtAvancer(")");
            v_inst.push_back(seqInst());     // On mémorise la séquence d'instruction
        }
        else {
            test = false;
        }
    }    
    if (m_lecteur.getSymbole()=="sinon") {
        testerEtAvancer("sinon");
        v_inst.push_back(seqInst()); // On mémorise la séquence d'instruction
    }
    testerEtAvancer("finsi");
    
    return new NoeudInstSiRiche(v_inst); // On renvoie un noeud Instruction SiRiche
}

Noeud* Interpreteur::instTantQue() {
    // <instTantQue> ::=tantque( <expression> ) <seqInst> fintantque
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "TantQue"
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise l'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence); // On renvoie un noeud Instruction TantQue
}

Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Repeter"
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst(); // On mémorise la séquence
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstRepeter(condition, sequence); // On renvoie un noeud Instruction Repeter

}

Noeud* Interpreteur::instPour() {
    // <instPour>    ::= pour( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Pour"
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = NULL; // On initialise affectation1 à NULL
    if (m_lecteur.getSymbole() == "<VARIABLE>")
        affectation1 = affectation(); // On mémorise l'affectation1
    testerEtAvancer(";");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(";");
    Noeud* affectation2 = NULL; // On initialise affectation2 à NULL
    if (m_lecteur.getSymbole() == "<VARIABLE>")
        affectation2 = affectation(); // On mémorise l'affectation2
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence
    testerEtAvancer("finpour");
    
    return new NoeudInstPour(affectation1, condition, affectation2, sequence); // On renvoie un noeud Instruction Pour
}

Noeud* Interpreteur::instEcrire() {
    // <instEcrire>  ::=ecrire( <expression> | <chaine> {, <expression> | <chaine> })
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Ecrire"
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    vector<Noeud *> v_ecrire;
    if(m_lecteur.getSymbole() == "<CHAINE>"){
        v_ecrire.push_back(m_table.chercheAjoute(m_lecteur.getSymbole())); // Le symbole est ajouté à la table et on le mémorise
        m_lecteur.avancer();
    }else {
        v_ecrire.push_back(expression()); // On ajoute l'expression dans le vecteur
    }
    while(m_lecteur.getSymbole() != ")"){ // Tant que la parenthèse n'est pas fermée, on lit les variables
        testerEtAvancer(",");
        if(m_lecteur.getSymbole() == "<CHAINE>"){
            v_ecrire.push_back(m_table.chercheAjoute(m_lecteur.getSymbole())); // Le symbole est ajouté à la table et on le mémorise
            m_lecteur.avancer();
        }else {
            v_ecrire.push_back(expression()); // On ajoute l'expression dans le vecteur
        }
    }
    
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstEcrire(v_ecrire); // On renvoie un noeud Instruction Ecrire
}

Noeud* Interpreteur::instLire() {
    // <instLire>  ::= lire( <variable> {, <variable> })
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Lire"
    vector<Noeud *> v_inst;
    testerEtAvancer("lire");
    testerEtAvancer("(");
     Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
    m_lecteur.avancer();
    v_inst.push_back(var); // On ajoute la variable dans le vecteur
    while (m_lecteur.getSymbole()!=")") {
        testerEtAvancer(",");
        Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
        m_lecteur.avancer();
        v_inst.push_back(var); // On ajoute la variable dans le vecteur
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    
    return new NoeudInstLire(v_inst); // On renvoie un noeud Instruction Lire
}

Noeud* Interpreteur::instSelon() {
    // <instSelon>     :: = selon(<variable>) [cas <expression> : <seqInst>] dafaut : <seqInst> finselon
    // On vérifie que ce qui est écrit respecte la syntaxe de l'instruction "Selon"
    Noeud* exp;
    Noeud* instruction;
    Noeud* var;
    vector<Noeud *> v_inst;
    vector<Noeud *> v_condition;
    
    testerEtAvancer("selon");
    testerEtAvancer("(");
    
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
        m_lecteur.avancer();
    }
    
    testerEtAvancer(")");
    testerEtAvancer("cas");
    exp = expression();
    v_condition.push_back(exp); // On ajoute l'expression dans le vecteur de conditions
    testerEtAvancer(":");
    instruction = seqInst(); // On mémorise la séquence d'instructions
    v_inst.push_back(instruction); // On ajoute la séquence d'instructions au vecteur d'instructions
    
    while(m_lecteur.getSymbole() == "cas"){
        testerEtAvancer("cas");
        exp = expression();
        v_condition.push_back(exp); // On ajoute l'expression au vecteur de condition
        testerEtAvancer(":");
        instruction = seqInst(); // On mémorise la séquence d'instruction
        v_inst.push_back(instruction); // On ajoute la séquence d'instructions dans le vecteur d'instructions
    }
    
    testerEtAvancer("defaut");
    exp = NULL; // exp est NULL car il n'y a pas de condition, on est dans le cas qui s'applique à tout ce qui ne fait pas partie des précédentes conditions
    v_condition.push_back(exp); // On ajoute l'expression au vecteur de condition
    testerEtAvancer(":");
    instruction = seqInst(); // On mémorise la séquence d'instructions
    v_inst.push_back(instruction); // On ajoute la séquence d'instructions au vecteur d'instructions
    
    testerEtAvancer("finselon");

    return new NoeudInstSelon(var,v_inst, v_condition); // On renvoie un noeud Instruction Selon
}