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

#include "siRicheTest.h"
#include "ArbreAbstrait.h"


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
    vector<Noeud*> vectInst;
    NoeudInstSiRiche noeudInstSiRiche(vectInst);
    CPPUNIT_ASSERT_MESSAGE("Construction du vecteur d'instructions correcte", noeudInstSiRiche(vectInst.size())=vectInst.size());
}

void siRicheTest::testExecuter() {
    int testSiRiche1 = 1;
    NoeudInstSiRiche siRiche;
    if (testSiRiche1==1) {
        CPPUNIT_ASSERT_MESSAGE("La variable testSiRiche1 est bien égale à 1, on rentre dans la condition", true);
    }
    int result = siRiche.executer();
}

void siRicheTest::testTraduitEnCPP() {
    ostream& cout;
    unsigned int indentation;
    NoeudInstSiRiche noeudInstSiRiche;
    noeudInstSiRiche.traduitEnCPP(cout, indentation);
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}

