/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "mathmlpresentationtest.h"
#include "mathmlpresentationlexer.h"
#include "expressionparser.h"
#include "expression.h"
#include <QTest>

using Analitza::Expression;

QTEST_MAIN(MathMLPresentationTest)
Q_DECLARE_METATYPE(AbstractLexer::TOKEN*)

MathMLPresentationTest::MathMLPresentationTest(QObject *parent)
 : QObject(parent)
{}

MathMLPresentationTest::~MathMLPresentationTest()
{}

void MathMLPresentationTest::initTestCase() {}
void MathMLPresentationTest::cleanupTestCase() {}

void MathMLPresentationTest::testSimple_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<AbstractLexer::TOKEN*>("output");
    
    QTest::newRow("1 value") << "<mn>123</mn>" << new AbstractLexer::TOKEN(ExpressionTable::tVal, 0, QStringLiteral("<cn>123</cn>"));
    QTest::newRow("1 variable") << "<mi>x</mi>" << new AbstractLexer::TOKEN(ExpressionTable::tId, 0, QStringLiteral("x"));
//     QTest::newRow("1 variable pi") << "<mi>&pi;</mi>" << new AbstractLexer::TOKEN(ExpressionTable::tVal, 0, "<ci>&pi;</ci>");
    
    QTest::newRow("1 operator") << "<mo>+</mo>" << new AbstractLexer::TOKEN(ExpressionTable::tAdd, 0, QString());
}

void MathMLPresentationTest::testSimple()
{
    QFETCH(QString, input);
    QFETCH(AbstractLexer::TOKEN*, output);
    
    MathMLPresentationLexer lex(input);
    
    int t=lex.lex();
    
    if(!lex.error().isEmpty())
        qDebug() << "error!" << lex.error();
    QVERIFY(lex.error().isEmpty());
    QCOMPARE(lex.current.type, output->type);
    QCOMPARE(t, output->type);
    QCOMPARE(lex.current.val, output->val);
    
    delete output;
}

void MathMLPresentationTest::testConversion_data()
{
    QTest::addColumn<QString>("mml_pr");
    QTest::addColumn<QString>("expression");
    
    QTest::newRow("square per square") <<
    "<math><mrow>"
        "<msup>"
            "<mi>x</mi>"
            "<mn>2</mn>"
        "</msup>"
        "<mo>*</mo>"
        "<msup>"
            "<mi>y</mi>"
            "<mn>2</mn>"
        "</msup>"
    "</mrow></math>" << "x^2*y^2";
    
    QTest::newRow("division") <<
    "<math>"
    "<mrow>"
        "<mfrac>"
            "<mrow>"
                "<mi>x</mi>"
                "<mo>+</mo>"
                "<msup>"
                    "<mi>y</mi>"
                    "<mn>2</mn>"
                "</msup>"
            "</mrow>"
            "<mrow>"
                "<mi>k</mi>"
                "<mo>+</mo>"
                "<mn>1</mn>"
            "</mrow>"
        "</mfrac>"
    "</mrow>"
    "</math>" << "(x+y^2)/(k+1)";
    
    QTest::newRow("powers") <<
    "<math>"
        "<mrow>"
            "<msup>"
                "<mn>2</mn>"
                "<msup>"
                    "<mn>2</mn>"
                    "<msup>"
                        "<mn>2</mn>"
                        "<mi>x</mi>"
                    "</msup>"
                "</msup>"
            "</msup>"
        "</mrow>"
    "</math>" << "2^(2^(2^x))";
    
    QTest::newRow("x+y^smth") <<
    "<math>"
        "<mrow>"
            "<mi>x</mi>"
            "<mo>+</mo>"
            "<msup>"
                "<mi>y</mi>"
                "<mfrac>"
                    "<mn>2</mn>"
                    "<mrow>"
                        "<mi>k</mi>"
                        "<mo>+</mo>"
                        "<mn>1</mn>"
                    "</mrow>"
                "</mfrac>"
            "</msup>"
        "</mrow>"
    "</math>" << "x+y^(2/(k+1))";
    
    QTest::newRow("a/(b/2)") <<
    "<math>"
        "<mrow>"
            "<mfrac>"
                "<mi>a</mi>"
                "<mrow>"
                    "<mi>b</mi>"
                    "<mo>/</mo>"
                    "<mn>2</mn>"
                "</mrow>"
            "</mfrac>"
        "</mrow>"
    "</math>" << "a/(b/2)";
    
    QTest::newRow("13th-test") <<
    "<math>"
    "<mrow>"
        "<msqrt>"
            "<mn>1</mn>"
            "<mo>+</mo>"
            "<msqrt>"
                "<mn>1</mn>"
                "<mo>+</mo>"
                "<msqrt>"
                    "<mn>1</mn>"
                    "<mo>+</mo>"
                    "<msqrt>"
                        "<mn>1</mn>"
                        "<mo>+</mo>"
                        "<msqrt>"
                            "<mn>1</mn>"
                            "<mo>+</mo>"
                            "<msqrt>"
                                "<mn>1</mn>"
                                "<mo>+</mo>"
                                "<msqrt>"
                                    "<mn>1</mn>"
                                    "<mo>+</mo>"
                                    "<mi>x</mi>"
                                "</msqrt>"
                            "</msqrt>"
                        "</msqrt>"
                    "</msqrt>"
                "</msqrt>"
            "</msqrt>"
        "</msqrt>"
    "</mrow>"
    "</math>" << "root(1+root(1+root(1+root(1+root(1+root(1+root(1+x, 2), 2), 2), 2), 2), 2), 2)";
}

void MathMLPresentationTest::testConversion()
{
    QFETCH(QString, mml_pr);
    QFETCH(QString, expression);
    
    MathMLPresentationLexer lex(mml_pr);
    ExpressionParser parser;
    bool corr=parser.parse(&lex);
    QString mathML=parser.mathML();
    
    if(!corr)
        qDebug() << "error!" << parser.error();
    QVERIFY(corr);
    QVERIFY(!mathML.isEmpty());
    
    Expression e(mathML, true);
    QVERIFY(e.isCorrect());
//     qDebug() << "fuuu" << mathML;
    QCOMPARE(expression, e.toString());
}

void MathMLPresentationTest::testToPresentation_data()
{
    QTest::addColumn<QString>("mml_pr");
    QTest::addColumn<QString>("expression");
    
    QTest::newRow("square per square") <<
    "<math><mrow>"
        "<msup>"
            "<mi>x</mi>"
            "<mn>2</mn>"
        "</msup>"
        "<mo>*</mo>"
        "<msup>"
            "<mi>y</mi>"
            "<mn>2</mn>"
        "</msup>"
    "</mrow></math>" << "x^2*y^2";
    
    QTest::newRow("division") <<
    "<math>"
    "<mrow>"
        "<mfrac>"
            "<mrow>"
                "<mi>x</mi>"
                "<mo>+</mo>"
                "<msup>"
                    "<mi>y</mi>"
                    "<mn>2</mn>"
                "</msup>"
            "</mrow>"
            "<mrow>"
                "<mi>k</mi>"
                "<mo>+</mo>"
                "<mn>1</mn>"
            "</mrow>"
        "</mfrac>"
    "</mrow>"
    "</math>" << "(x+y^2)/(k+1)";
    
    QTest::newRow("powers") <<
    "<math>"
        "<mrow>"
            "<msup>"
                "<mn>2</mn>"
                "<msup>"
                    "<mn>2</mn>"
                    "<msup>"
                        "<mn>2</mn>"
                        "<mi>x</mi>"
                    "</msup>"
                "</msup>"
            "</msup>"
        "</mrow>"
    "</math>" << "2^(2^(2^x))";
    
    QTest::newRow("x+y^smth") <<
    "<math>"
        "<mrow>"
            "<mi>x</mi>"
            "<mo>+</mo>"
            "<msup>"
                "<mi>y</mi>"
                "<mfrac>"
                    "<mrow><mn>2</mn></mrow>"
                    "<mrow>"
                        "<mi>k</mi>"
                        "<mo>+</mo>"
                        "<mn>1</mn>"
                    "</mrow>"
                "</mfrac>"
            "</msup>"
        "</mrow>"
    "</math>" << "x+y^(2/(k+1))";
    
    QTest::newRow("13th-test") <<
    "<math>"
    "<mrow>"
        "<msqrt>"
            "<mn>1</mn>"
            "<mo>+</mo>"
            "<msqrt>"
                "<mn>1</mn>"
                "<mo>+</mo>"
                "<msqrt>"
                    "<mn>1</mn>"
                    "<mo>+</mo>"
                    "<msqrt>"
                        "<mn>1</mn>"
                        "<mo>+</mo>"
                        "<msqrt>"
                            "<mn>1</mn>"
                            "<mo>+</mo>"
                            "<msqrt>"
                                "<mn>1</mn>"
                                "<mo>+</mo>"
                                "<msqrt>"
                                    "<mn>1</mn>"
                                    "<mo>+</mo>"
                                    "<mi>x</mi>"
                                "</msqrt>"
                            "</msqrt>"
                        "</msqrt>"
                    "</msqrt>"
                "</msqrt>"
            "</msqrt>"
        "</msqrt>"
    "</mrow>"
    "</math>" << "root(1+root(1+root(1+root(1+root(1+root(1+root(1+x, 2), 2), 2), 2), 2), 2), 2)";
    
    QTest::newRow("normal function") <<
    "<math><mrow><mi>sin</mi><mo> &ApplyFunction; </mo><mfenced><mi>x</mi></mfenced></mrow></math>" << "sin(x)";
    
    QTest::newRow("piecewise") <<
    "<math><mrow><mrow><mo stretchy='true'> { </mo><mtable columnalign='left left'><mtr><mtd><mi>x</mi></mtd><mtd><mtext>if </mtext><mi>x</mi><mo>=</mo><mn>3</mn></mtd></mtr><mtr><mtd><mn>33</mn></mtd><mtd><mtext>otherwise</mtext></mtd></mtr></mtable></mrow></mrow></math>"
        << "piecewise { x=3 ? x, ? 33 }";
}

void MathMLPresentationTest::testToPresentation()
{
    if(QLatin1String(QTest::currentDataTag())==QLatin1String("piecewise") || QLatin1String(QTest::currentDataTag())==QLatin1String("normal function"))
        QSKIP("need to find time to fix piecewise and normal function");
    QFETCH(QString, mml_pr);
    QFETCH(QString, expression);
    
    Expression e(expression, false);
    if(!e.isCorrect())
        qDebug() << "error:" << e.error() << expression;
    QString mmlcnt=e.toMathML();
    
    QVERIFY(e.isCorrect());
    QCOMPARE(e.toString(), expression);
    QCOMPARE(e.toMathMLPresentation(), mml_pr);
    
    MathMLPresentationLexer lex(mml_pr);
    ExpressionParser parser;
    bool corr=parser.parse(&lex);
    if(!corr)
        qDebug() << parser.error();
    QVERIFY(corr);
    QCOMPARE(mmlcnt, parser.mathML());
}


