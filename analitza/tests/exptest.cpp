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

#include "exptest.h"
#include "explexer.h"
#include "expressionparser.h"
#include <QTest>
#include <QDebug>

QTEST_MAIN( ExpTest )
ExpTest::ExpTest(QObject *parent)
 : QObject(parent)
{}

ExpTest::~ExpTest()
{}

Q_DECLARE_METATYPE(QList<uchar>)

void ExpTest::initTestCase() {}

void ExpTest::cleanupTestCase() {}

void ExpTest::testSimple_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    
    QTest::newRow("1 value") << "1" << "<math><cn>1</cn></math>";
    QTest::newRow("1.5 value") << "1.5" << "<math><cn type='real'>1.5</cn></math>";
    QTest::newRow(".5 value") << ".5" << "<math><cn type='real'>.5</cn></math>";
//     QTest::newRow("1. value") << "1." << "<math><cn type='real'>1.</cn></math>";
    QTest::newRow("exp_vals") << "1e1+1e-2+-1e3" << "<math><apply><plus /><apply><plus /><cn type='real'>1e1</cn><cn type='real'>1e-2</cn></apply><apply><minus /><cn type='real'>1e3</cn></apply></apply></math>";
    
    QTest::newRow("addition") << "1+2" << "<math><apply><plus /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("equality") << "1=2" << "<math><apply><eq /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("subtraction") << "2-3" << "<math><apply><minus /><cn>2</cn><cn>3</cn></apply></math>";
    QTest::newRow("unary minus alone") << "-3" << "<math><apply><minus /><cn>3</cn></apply></math>";
    QTest::newRow("x*unary minus") << "x*(-3)" << "<math><apply><times /><ci>x</ci>"
                                              "<apply><minus /><cn>3</cn></apply></apply></math>";
    QTest::newRow("unary minus*x") << "-3*x" << "<math><apply><times /><apply><minus />"
                                              "<cn>3</cn></apply><ci>x</ci></apply></math>";
    
    QTest::newRow("assignation") << "x:=2+3" << "<math><declare><ci>x</ci><apply><plus /><cn>2</cn>"
                                                    "<cn>3</cn></apply></declare></math>";
    QTest::newRow("simple expression") << "2+1-3" << "<math><apply><minus /><apply><plus />"
                                        "<cn>2</cn><cn>1</cn></apply><cn>3</cn></apply></math>";
    QTest::newRow("times") << "1*2" << "<math><apply><times /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("power") << "1^2" << "<math><apply><power /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("power") << "1**2"<< "<math><apply><power /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("times") << "1/2" << "<math><apply><divide /><cn>1</cn><cn>2</cn></apply></math>";
    QTest::newRow("priority") << "2+3*5" << "<math><apply><plus /><cn>2</cn><apply><times />"
                                            "<cn>3</cn><cn>5</cn></apply></apply></math>";
    QTest::newRow("unicode y*x**2") << "y*x²" << "<math><apply><times /><ci>y</ci><apply><power /><ci>x</ci><cn>2</cn></apply></apply></math>"; //x**2y
    QTest::newRow("unicode x**2*y") << "x²*y" << "<math><apply><times /><apply><power /><ci>x</ci><cn>2</cn></apply><ci>y</ci></apply></math>"; //x**2y
    
    QTest::newRow("function") << "func(x, y)" << "<math><apply><ci type='function'>func</ci>"
                                                "<ci>x</ci><ci>y</ci></apply></math>";
    QTest::newRow("function_np") << "sin x/x" << "<math><apply><divide /><apply><sin /><ci>x</ci></apply><ci>x</ci></apply></math>";
    QTest::newRow("block") << "blk{x, y}" << "<math><blk><ci>x</ci><ci>y</ci></blk></math>";
    
    QTest::newRow("sum") << "sum(x : x=1..10)" << "<math>"
            "<apply><sum /><bvar><ci>x</ci></bvar><uplimit><cn>10</cn></uplimit><downlimit>"
            "<cn>1</cn></downlimit><ci>x</ci></apply></math>";
    QTest::newRow("x*sum") << "x*sum(x : x=1..10)" <<
            "<math><apply><times /><ci>x</ci><apply><sum /><bvar><ci>x</ci></bvar>"
            "<uplimit><cn>10</cn></uplimit><downlimit><cn>1</cn></downlimit><ci>x</ci></apply></apply></math>";
    QTest::newRow("piecewise") << "piecewise{x?a, ?b}" << "<math><piecewise><piece><ci>a</ci><ci>x</ci></piece>"
                                                        "<otherwise><ci>b</ci></otherwise></piecewise></math>";
    QTest::newRow("piecewise2") << "piecewise{?b}" << "<math><piecewise><otherwise><ci>b</ci></otherwise></piecewise></math>";
    
    QTest::newRow("piecewise2") << "fib:=n->piecewise { eq(n,0)?0, eq(n,1)?1, ?fib(n-1)+fib(n-2) }" << "<math><declare><ci>fib</ci><lambda><bvar><ci>n</ci></bvar><piecewise><piece><cn>0</cn><apply><eq /><ci>n</ci><cn>0</cn></apply></piece><piece><cn>1</cn><apply><eq /><ci>n</ci><cn>1</cn></apply></piece><otherwise><apply><plus /><apply><ci type='function'>fib</ci><apply><minus /><ci>n</ci><cn>1</cn></apply></apply><apply><ci type='function'>fib</ci><apply><minus /><ci>n</ci><cn>2</cn></apply></apply></apply></otherwise></piecewise></lambda></declare></math>";
    
    QTest::newRow("lambda") << "f:=x->(3)" << "<math><declare><ci>f</ci><lambda><bvar><ci>x</ci></bvar><cn>3</cn></lambda></declare></math>";
    QTest::newRow("lambda2") << "f:=(x, y)->(x+y)"
                << "<math><declare><ci>f</ci><lambda><bvar><ci>x</ci></bvar><bvar><ci>y</ci></bvar><apply><plus /><ci>x</ci><ci>y</ci></apply></lambda></declare></math>";
    QTest::newRow("lambda3") << "g:=y->y" << "<math><declare><ci>g</ci><lambda><bvar><ci>y</ci></bvar><ci>y</ci></lambda></declare></math>";
    QTest::newRow("unary minus") << "1*(-2)" << "<math><apply><times /><cn>1</cn><apply>"
                                                "<minus /><cn>2</cn></apply></apply></math>";
    QTest::newRow("bounds and !limit") << "func(x:x)" << "<math><apply><ci type='function'>func</ci>"
                                    "<bvar><ci>x</ci></bvar><ci>x</ci></apply></math>";
    QTest::newRow("bounds and limit") << "func(x+y : x=0..1)" << 
                "<math><apply><ci type='function'>func</ci>"
                "<bvar><ci>x</ci></bvar><uplimit><cn>1</cn></uplimit><downlimit><cn>0</cn></downlimit>"
                "<apply><plus /><ci>x</ci><ci>y</ci></apply></apply></math>";
    QTest::newRow("bounds and limit") << "card(vector { x, y, z })"
                << "<math><apply><card /><vector><ci>x</ci><ci>y</ci><ci>z</ci></vector></apply></math>";
    QTest::newRow("lambda call") << "(x->x+2)(2)" << "<math><apply><lambda><bvar><ci>x</ci></bvar><apply><plus /><ci>x</ci><cn>2</cn></apply></lambda><cn>2</cn></apply></math>";
    QTest::newRow("union") << "union(list{1}, list {2})" << "<math><apply><union /><list><cn>1</cn></list><list><cn>2</cn></list></apply></math>";
    QTest::newRow("list") << "list{}" << "<math><list /></math>";
    QTest::newRow("minus order") << "-3^2" << "<math><apply><minus /><apply><power /><cn>3</cn><cn>2</cn></apply></apply></math>";
    QTest::newRow("minus order") << "v[3]" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    
    QTest::newRow("comment1") << "//a\nv[3]" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    QTest::newRow("comment2") << "//a//v[3]" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    QTest::newRow("comment3") << "v//a//[3]" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    QTest::newRow("comment4") << "v[3]//a//" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    QTest::newRow("comment5") << "v[3]//a\n" << "<math><apply><selector /><cn>3</cn><ci>v</ci></apply></math>";
    
    QTest::newRow("string1") << "\"hola\"" << "<math><cs>hola</cs></math>";
    QTest::newRow("string2") << "\"a<b\"" << "<math><cs>a&lt;b</cs></math>";
//     QTest::newRow("string3") << "\"a\\nb\"" << "<math><cs>a\nb</cs></math>";
    
    QTest::newRow("pipe1")  << "3 | sin" << "<math><apply><sin /><cn>3</cn></apply></math>";
    QTest::newRow("pipe2") << "3|sin|cos|tan" << "<math><apply><tan /><apply><cos /><apply><sin /><cn>3</cn></apply></apply></apply></math>";
    QTest::newRow("pipe3") << "3|f|g" << "<math><apply><ci type='function'>g</ci><apply><ci type='function'>f</ci><cn>3</cn></apply></apply></math>";
    QTest::newRow("pipe4") << "3| (x->f(3,x)) |g" << "<math><apply><ci type='function'>g</ci><apply><lambda><bvar><ci>x</ci></bvar><apply><ci type='function'>f</ci><cn>3</cn><ci>x</ci></apply></lambda><cn>3</cn></apply></apply></math>";
}

void ExpTest::testSimple()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    
    ExpLexer lex(input);
    ExpressionParser parser;
    bool corr=parser.parse(&lex);
    
    if(!parser.error().isEmpty())
        qDebug() << ">>> " << parser.mathML() << "errors:" << parser.error();
    QVERIFY(corr);
    QVERIFY(parser.error().isEmpty());
    QCOMPARE(parser.mathML(), output);
}

void ExpTest::testExp_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    QString fourX=QStringLiteral("<math><apply><plus /><ci>x</ci><ci>x</ci><ci>x</ci><ci>x</ci></apply></math>");

    //FIXME: Repetition not supported
//     QTest::newRow("simple expression") << "x+x+x+x" << fourX;
    QTest::newRow("composed expression") << QStringLiteral("3²")
            << "<math><apply><power /><cn>3</cn><cn>2</cn></apply></math>";
    QTest::newRow("plus operator in plus() form") << "plus(x,x,x,x)" << fourX;
    QTest::newRow("sum") << "x*sum(x : x=1..10)" << "<math><apply><times /><ci>x</ci>"
            "<apply><sum /><bvar><ci>x</ci></bvar><uplimit><cn>10</cn></uplimit><downlimit>"
            "<cn>1</cn></downlimit><ci>x</ci></apply></apply></math>";
    
    QTest::newRow("lol") << "times((x),(y))" << "<math><apply><times /><ci>x</ci><ci>y</ci></apply></math>";
    QTest::newRow("lol1") << "times((x),(y),((z)))" << "<math><apply><times /><ci>x</ci><ci>y</ci><ci>z</ci></apply></math>";
    QTest::newRow("notimes") << "2x" << "<math><apply><times /><cn>2</cn><ci>x</ci></apply></math>";
//     QTest::newRow("notimes_sin") << "2(sin x)" <<
//         "<math><apply><times /><cn>2</cn><apply><sin /><ci>x</ci></apply></apply></math>";
//     QTest::newRow("notimes_sin") << "2sin(x)" <<
//         "<math><apply><times /><cn>2</cn><apply><sin /><ci>x</ci></apply></apply></math>";
}

void ExpTest::testExp()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    
    ExpLexer lex(input);
    ExpressionParser parser;
    bool corr=parser.parse(&lex);
    if(!parser.error().isEmpty())
        qDebug() << "errors:" << parser.error();
    QVERIFY(parser.error().isEmpty() && corr);
    QCOMPARE(parser.mathML(), output);
}

void ExpTest::testLength_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QList<uchar> >("lengths");
    
    QList<uchar> lengths;
    lengths << 1 << 1 << 1 << 1 << 1;
    QTest::newRow("simple addition") << "2+2+2" << lengths;
    
    lengths.clear();
    lengths << 2 << 3;
    QTest::newRow("power with utf composition") << QStringLiteral("22²²²") << lengths;
    
    lengths.clear();
    lengths << 1;
    QTest::newRow("utf composed variable") << QStringLiteral("ç") << lengths;
    
    lengths.clear();
    lengths << 1 << 1;
    QTest::newRow("no times operator") << QStringLiteral("2x") << lengths;
}

void ExpTest::testLength()
{
    QFETCH(QString, input);
    QFETCH(QList<uchar>, lengths);
    
    ExpLexer lex=ExpLexer(input);
    
    int current=0;
    while(lex.lex()!=0/*EOF*/) {
        QVERIFY(lex.current.type>0);
        QCOMPARE((int) lengths[current], (int) lex.current.len);
        
        current++;
    }
}

void ExpTest::testCorrection_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("correct");
    
    QTest::newRow("stack killing") << "k+++k" << false;
    QTest::newRow("more stack killing") << "k-++k" << false;
    QTest::newRow("omartinez bug") << "piecewise { gt(x,23)?a }" << true;
    QTest::newRow("wrong coma") << "2+2," << false;
    QTest::newRow("wrong token") << "q-<" << false;
    QTest::newRow("misplaced declaration") << "2+2+(w:=2)" << false;
}

void ExpTest::testCorrection()
{
    QFETCH(QString, input);
    QFETCH(bool, correct);
    
    ExpLexer lex(input);
    ExpressionParser parser;
    bool corr=parser.parse(&lex);
    QCOMPARE(corr, correct);
    QCOMPARE(parser.error().isEmpty(), correct);
}


