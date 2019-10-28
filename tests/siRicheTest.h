/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   siRicheTest.h
 * Author: bretonem
 *
 * Created on Oct 28, 2019, 3:29:38 PM
 */

#ifndef SIRICHETEST_H
#define SIRICHETEST_H

#include <cppunit/extensions/HelperMacros.h>

class siRicheTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(siRicheTest);

    CPPUNIT_TEST(testNoeudInstSiRiche);
    CPPUNIT_TEST(testExecuter);
    CPPUNIT_TEST(testTraduitEnCPP);

    CPPUNIT_TEST_SUITE_END();

public:
    siRicheTest();
    virtual ~siRicheTest();
    void setUp();
    void tearDown();

private:
    void testNoeudInstSiRiche();
    void testExecuter();
    void testTraduitEnCPP();

};

#endif /* NEWTESTCLASS_H */

