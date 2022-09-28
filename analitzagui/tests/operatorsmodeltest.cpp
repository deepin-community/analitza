/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "operatorsmodeltest.h"
#include <analitza/expression.h>
#include <analitza/analyzer.h>
#include <analitza/variables.h>
#include <QTest>
#include <cmath>
#include <analitza/analitzautils.h>
#include <analitza/value.h>

using namespace std;
using Analitza::Expression;

QTEST_GUILESS_MAIN( OperatorsModelTest )

OperatorsModelTest::OperatorsModelTest(QObject *parent)
    : QObject(parent)
{}

OperatorsModelTest::~OperatorsModelTest() {}
void OperatorsModelTest::initTestCase() {}
void OperatorsModelTest::cleanupTestCase() {}

void OperatorsModelTest::testExamples_data()
{
    QTest::addColumn<int>("i");
    for(int i=0; i<m.rowCount(); i++) {
        QModelIndex idx(m.index(i, 0));
        QTest::newRow(qPrintable(idx.data().toString())) << i;
    }
}

void OperatorsModelTest::testExamples()
{
    QFETCH(int, i);
    QModelIndex idx(m.index(i, 0));
    
    QModelIndex nameIdx, descriptionIdx, sampleIdx, exampleIdx;
    nameIdx = idx.sibling(idx.row(), 0);
    descriptionIdx = idx.sibling(idx.row(), 1);
    sampleIdx = idx.sibling(idx.row(), 2);
    exampleIdx = idx.sibling(idx.row(), 3);
    
    QString name=m.data(nameIdx).toString();
    QString description=m.data(descriptionIdx).toString();
    QString sample=m.data(sampleIdx).toString();
    QString example=m.data(exampleIdx).toString();
    
//         qDebug() << "testing: " << name << example;
    
    QVERIFY(!name.isEmpty());
    QVERIFY(!description.isEmpty());
    QVERIFY(!sample.isEmpty());
    QVERIFY(!example.isEmpty());
    Expression ex(example, false);
    if(!ex.isCorrect())
        qDebug() << "error" << example << ex.error();
    QCOMPARE(ex.toString(), example);
    QVERIFY(!ex.toMathMLPresentation().isEmpty());
    ex = ex.lambdaBody();
    
    Analitza::Analyzer a;
    a.setExpression(ex);
    if(!a.isCorrect()) qDebug() << example << "1. error" << a.errors();// QVERIFY(a.isCorrect());
    
    a.simplify();
    if(!a.isCorrect()) qDebug() << example << "2. error" << a.errors();// QVERIFY(a.isCorrect());
    a.variables()->modify(QStringLiteral("x"), 0.1);
    a.setExpression(ex);
    if(!a.isCorrect()) qDebug() << example << "2.1 type error" << ex.toString() << a.errors();// QVERIFY(a.isCorrect());
    
    bool typecorr = a.type().canReduceTo(Analitza::ExpressionType::Value);
    if(!typecorr)
        qDebug() << example << "2.2 error" << ex.toString() << a.type().toString();
    QVERIFY(typecorr);
    
    Expression e = a.calculate();
    if(!a.isCorrect()) qDebug() << example << "3. error" << a.errors();// QVERIFY(a.isCorrect());
    if(!e.isCorrect()) qDebug() << example << "4. error" << e.error(); // QVERIFY(e.isCorrect());
    
    e = a.evaluate();
    if(!a.isCorrect()) qDebug() << example << "5. error" << a.errors();// QVERIFY(a.isCorrect());
    if(!e.isCorrect()) qDebug() << example << "6. error" << e.error(); // QVERIFY(e.isCorrect());
    QVERIFY(!a.expression().toMathMLPresentation().isEmpty());
}


