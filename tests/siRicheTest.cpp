/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   siRicheTest.cpp
 * Author: bretonem
 *
 * Created on Oct 28, 2019, 3:29:38 PM
 */

#include "../tests/siRicheTest.h"
#include "../ArbreAbstrait.h"
#include "../Interpreteur.h"


CPPUNIT_TEST_SUITE_REGISTRATION(siRicheTest);

siRicheTest::siRicheTest() {
}

siRicheTest::~siRicheTest() {
}

void siRicheTest::setUp() {
}

void siRicheTest::tearDown() {
}

void siRicheTest::testNoeudInstSiRiche() {
    string nomFich;
    nomFich = "/users/info/etu-s3/viellarb/NetBeansProjects/M3105-TP5/testSiRiche.txt";
    ifstream fichier(nomFich.c_str());
    Interpreteur interpreteur(fichier);
    CPPUNIT_ASSERT_NO_THROW(interpreteur.analyse());
        
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Instruction Interprétation problème",interpreteur.getArbre());
    
    string nomFich2;
    nomFich2 = "/users/info/etu-s3/viellarb/NetBeansProjects/M3105-TP5/programmeAvecErreur.txt";
    ifstream fichier2(nomFich2.c_str());
    Interpreteur interpreteur2(fichier);
    CPPUNIT_ASSERT_THROW(interpreteur2.analyse(),InterpreteurException);

    }

void siRicheTest::testExecuter() {
    string nomFich;
    nomFich = "/users/info/etu-s3/viellarb/NetBeansProjects/M3105-TP5/testSiRiche.txt";
    ifstream fichier(nomFich.c_str());
    if (fichier.fail()) throw FichierException();
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    
    Noeud* arbre = interpreteur.getArbre(); //On interprète le fichier txt
    
    CPPUNIT_ASSERT_NO_THROW(interpreteur.getArbre()->executer());
    TableSymboles table = interpreteur.getTable();
    for (int i= 0;i<table.getTaille();i++){
        if (table[i].getChaine()=="testSi1" || table[i].getChaine()=="testSi2" || table[i].getChaine()=="testSi3" || table[i].getChaine()=="testSi4" || table[i].getChaine()=="testSi5"){
            SymboleValue symbole = table[i];
            int valeurS = symbole.executer();
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Le test est réussis",valeurS,1);
        }
    }
    
    string nomFich2;
    nomFich2 = "/users/info/etu-s3/viellarb/NetBeansProjects/M3105-TP5/programmeAvecErreurExec.txt";
    ifstream fichier2(nomFich2.c_str());
    if (fichier2.fail()) throw FichierException();
    Interpreteur interpreteur2(fichier2);
    interpreteur2.analyse();
        
    CPPUNIT_ASSERT_THROW(interpreteur2.getArbre()->executer(),InterpreteurException);
    
    
}

void siRicheTest::testTraduitEnCPP() {
        string nomFich;
        nomFich = "/users/info/etu-s3/viellarb/NetBeansProjects/M3105-TP5/testSiRiche.txt";
        ifstream fichier(nomFich.c_str());
        if (fichier.fail()) throw FichierException();
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        
        string fichCPP;
        fichCPP = "tradCPP.txt";
        fstream fichierCPP(fichCPP.c_str());
        
        CPPUNIT_ASSERT_NO_THROW(interpreteur.traduitEnCPP(fichierCPP,0));
     
}

