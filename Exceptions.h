/* 
 * File:   Exceptions.h
 * Author: martin
 *
 * Created on 7 décembre 2014, 19:08
 */

#ifndef EXCEPTIONS_H
#define	EXCEPTIONS_H
#include <exception>
#include <string>
using namespace std;

// Classe mère de toutes les exceptions de l'interpréteur
class InterpreteurException : public exception {
public:
    const char * what() const throw() override {
        return "Exception Interpreteur";
    }
};

// Exception levée si le fichier choisi ne s'est pas ouvert correctement
class FichierException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Ouverture Fichier Impossible";
    }
};

// Exception levée s'il y a une faute de syntaxe dans l'écriture d'une instruction
class SyntaxeException : public InterpreteurException {
public:
    SyntaxeException(const char * message = NULL) : m_message(message) {}
    const char * what() const throw() override {
        return m_message;
    }
private :
    const char* m_message;
};

// Exception levée si la valeur entrée n'est pas correcte (exemple : float)
class IndefiniException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Valeur Indéfinie";
    }
};

// Exception levée si on divise une valeur par 0
class DivParZeroException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Division par 0";
    }
};

// Exception levée si une opération interdite est utilisée
class OperationInterditeException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Operation Interdite sur un noeud";
    }
};

#endif	/* EXCEPTIONS_H */

