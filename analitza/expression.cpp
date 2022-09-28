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
#include "expression.h"

#include <QDomElement>
#include <QCoreApplication>

#include "explexer.h"
#include "expressionparser.h"
#include "container.h"
#include "value.h"
#include "vector.h"
#include "variable.h"
#include "stringexpressionwriter.h"
#include "htmlexpressionwriter.h"
#include "mathmlexpressionwriter.h"
#include "mathmlpresentationexpressionwriter.h"
#include "list.h"
#include "analitzautils.h"
#include "apply.h"
#include "customobject.h"

#include "matrix.h"

using namespace Analitza;

class Expression::ExpressionPrivate : public QSharedData
{
public:
    ExpressionPrivate(Object* t) : m_tree(t) {}
    
    bool canAdd(const Object* where, const Object* branch);
    bool check(const Container* c);
    bool check(const Apply* c);
    
    Object* branch(const QDomElement& elem);
    
    template <class T, class Tcontained = Object>
    T* addElements(T* v, const QDomElement* elem)
    {
        QDomNode n = elem->firstChild();
        while(!n.isNull()) {
            if(n.isElement()) {
                Object* ob= branch(n.toElement()); //TODO: We should do that better
                
                if(ob && canAdd(v, ob)) {
                    v->appendBranch(static_cast<Tcontained*>(ob));
                } else {
//                     delete ob;
                    delete v;
                    return nullptr;
                }
            }
            n = n.nextSibling();
        }
        return v;
    }
    static List* listFromString(const QString& text);
    
    Object* m_tree;
    QStringList m_err;
};

Expression::Expression()
    : d(new ExpressionPrivate(nullptr))
{}

Expression::Expression(Object * o)
    : d(new ExpressionPrivate(o))
{}

Expression::Expression(const Cn & e)
    : d(new ExpressionPrivate(nullptr))
{
    d->m_tree = new Cn(e);
}

Expression::Expression(const Expression & e)
    : d(new ExpressionPrivate(nullptr))
{
    d->m_err=e.d->m_err;
    if(e.isCorrect() && e.d->m_tree)
        d->m_tree = e.d->m_tree->copy();
}

Expression::Expression(const QString & exp, bool mathml)
    : d(new ExpressionPrivate(nullptr))
{
    if(mathml)
        setMathML(exp);
    else
        setText(exp);
}

Expression::~Expression()
{
    Q_ASSERT(d);
    delete d->m_tree;
}

Expression Expression::operator=(const Expression & e)
{
    if(this != &e) {
        delete d->m_tree;
        
        if(e.d->m_tree)
            d->m_tree = e.d->m_tree->copy();
        else
            d->m_tree=nullptr;
        
        d->m_err = e.d->m_err;
    }
    return *this;
}

bool Expression::setText(const QString & exp)
{
    if(exp.isEmpty()) {
        delete d->m_tree;
        d->m_tree = nullptr;
        return true;
    }
    d->m_err.clear();
    ExpLexer lex(exp);
    ExpressionParser parser;
    m_comments = parser.comments();
    bool corr=parser.parse(&lex);
    
    if(corr)
        setMathML(parser.mathML());
    else
        d->m_err << parser.error();
    
    return corr;
}

bool Expression::ExpressionPrivate::check(const Apply* c)
{
    bool ret=true;
    Operator op=c->firstOperator();
    Operator::OperatorType opt=op.operatorType();
    int cnt=c->countValues();
    
    if(((op.nparams()<0 && cnt<=1) || (op.nparams()>-1 && cnt!=op.nparams())) && (opt!=Operator::minus || cnt==0) && opt!=Operator::function)
    {
        if(op.nparams()<0)
            m_err << QCoreApplication::tr("<em>%1</em> needs at least 2 parameters").arg(op.toString());
        else
            m_err << QCoreApplication::tr("<em>%1</em> requires %2 parameters")
                                            .arg(op.toString())
                                            .arg(op.nparams());
        ret=false;
    }
    
    if(op.isBounded() && !c->hasBVars()) {
        m_err << QCoreApplication::tr("Missing boundary for '%1'").arg(op.toString());
    } else if(!op.isBounded() && c->hasBVars()) {
        m_err << QCoreApplication::tr("Unexpected bounding for '%1'").arg(op.toString());
    }
    
    if(op.isBounded() && op!=Operator::diff) {
        if(!(c->ulimit() && c->dlimit()) && !c->domain()) {
            m_err << QCoreApplication::tr("<em>%1</em> missing bounds on '%2'").arg(c->bvarStrings().join(QStringLiteral(" ")), op.toString());
        }
    }
    
    return ret;
}

List* Expression::ExpressionPrivate::listFromString(const QString& text)
{
    List* l = new List;
    for(int i=0; i<text.size(); ++i)
        l->appendBranch(new Cn(text[i]));
    return l;
}

bool Expression::ExpressionPrivate::check(const Container* c)
{
    bool ret=true;
    
    switch(c->containerType()) {
        case Container::declare:
            if(c->m_params.size()!=2) {
                m_err << QCoreApplication::tr("Wrong declare");
                ret=false;
            }
            break;
        default:
            //should never do anything here,
            break;
    }
    
    if(c->isEmpty() && c->containerType()!=Container::math) {
        m_err << QCoreApplication::tr("Empty container: %1").arg(c->tagName());
        ret=false;
    }
    
    return ret;
}

bool Expression::ExpressionPrivate::canAdd(const Object* where, const Object* branch)
{
    Q_ASSERT(where && branch);
    bool correct=true;
    
    if(branch->type()==Object::container) {
        const Container* cBranch=static_cast<const Container*>(branch);
        if(cBranch->containerType()==Container::piece || cBranch->containerType()==Container::otherwise) {
            bool isPiecewise=where->type()==Object::container
                && static_cast<const Container*>(where)->containerType()==Container::piecewise;
            if(!isPiecewise) {
                m_err << QCoreApplication::translate("there was a conditional outside a condition structure",
                                "We can only have conditionals inside piecewise structures.");
                correct=false;
            }
        } else if(cBranch->containerType()==Container::bvar && (where->isContainer() || where->isApply())) {
            //we don't want 2 bvar in the same element
            QStringList bvars;
            if(where->isContainer())
                bvars=static_cast<const Container*>(where)->bvarStrings();
            else
                bvars=static_cast<const Apply*>(where)->bvarStrings();
            
            const Ci* var= static_cast<const Ci*>(cBranch->m_params.first());
            
            if(bvars.contains(var->name())) {
                correct=false;
                m_err << QCoreApplication::tr("Cannot have two parameters with the same name like '%1'.").arg(var->name());
            }
        }
    }
    
    if(where->type()==Object::container) {
        const Container* cWhere=static_cast<const Container*>(where);
        
        if(cWhere->containerType()==Container::piecewise) {
            bool isCondition=false;
            if(branch->type()==Object::container) {
                Container::ContainerType ct=static_cast<const Container*>(branch)->containerType();
                isCondition=ct==Container::piece || ct==Container::otherwise;
                
                if(cWhere->extractType(Container::otherwise)) {
                    m_err << QCoreApplication::translate("this is an error message. otherwise is the else in a mathml condition",
                        "The <em>otherwise</em> parameter should be the last one");
                    correct=false;
                }
                
            }
            
            if(!isCondition) {
                m_err << QCoreApplication::translate("there was an element that was not a conditional inside a condition",
                                    "%1 is not a proper condition inside the piecewise").arg(branch->toString());
                correct=false;
            }
            
        } else if(cWhere->containerType()==Container::declare && cWhere->isEmpty() && branch->type()!=Object::variable) {
            m_err << QCoreApplication::tr("We can only declare variables");
            correct=false;
        }
        
        if(cWhere->containerType()==Container::bvar) {
            if(branch->type()!=Object::variable) {
                m_err << QCoreApplication::tr("We can only have bounded variables");
                correct=false;
            }
        }
    } else 
        if(where->type()==Object::matrix) {
            if(branch->type()==Object::matrixrow) {
                const Matrix *matrix = static_cast<const Matrix*>(where);
                const MatrixRow *matrixrow = static_cast<const MatrixRow*>(branch);
                
                if (matrixrow->size() == 0) {
                    m_err << QCoreApplication::tr("Do not want empty matrixrow elements");
                    correct=false;
                }
                
                if (matrix->rowCount() != 0)
                    if (static_cast<MatrixRow*>(matrix->rows().at(0))->size() != matrixrow->size()) {
                        m_err << QCoreApplication::tr("All matrixrow elements must have the same size");
                        correct=false;
                    }
            } else {
                m_err << QCoreApplication::tr("The matrix must contain only matrixrow elements");
                correct=false;
            }
        }
    
    Q_ASSERT(correct || !m_err.isEmpty());
    
    return correct;
}

static void variableDepth(Object* o, int& next, const QMap<QString, int>& scope, const Object* parent=nullptr)
{
    Q_ASSERT(o);
    switch(o->type()) {
        case Object::variable: {
            Ci *i = (Ci*) o;
            QMap<QString, int>::const_iterator it = scope.constFind(i->name());
            i->setBVarDepth(it==scope.constEnd() 
                            ? -1
                            : *it);
            
        }    break;
        case Object::vector: {
            Vector *v=(Vector*) o;
            for(Vector::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
                variableDepth(*it, next, scope);
        }    break;
        case Object::matrix: {
            Matrix *v=(Matrix*) o;
            for(Matrix::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
                variableDepth(*it, next, scope);
        }    break;
        case Object::matrixrow: {
            MatrixRow *v=(MatrixRow*) o;
            for(MatrixRow::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
                variableDepth(*it, next, scope);
        }    break;
        case Object::list: {
            List *v=(List*) o;
            for(List::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
                variableDepth(*it, next, scope);
            
        }    break;
        case Object::container: {
            const Container *c = (const Container*) o;
            
            QMap<QString, int> newScope=scope;
            
            Container::const_iterator it=c->m_params.constBegin(), itEnd=c->m_params.constEnd();
            if(c->containerType()==Container::declare) {
                Ci* var = static_cast<Ci*>(*it);
                ++it;
                
                if((*it)->isContainer() && static_cast<const Container*>(*it)->containerType()==Container::lambda) {
                    newScope.insert(var->name(), next);
                    var->setBVarDepth(next++);
                }
            } else if(c->containerType()==Container::lambda &&
                !(parent && parent->isContainer() && static_cast<const Container*>(parent)->containerType()==Container::declare)) {
                    ++next;
            }
            
            foreach(Ci* bvar, c->bvarCi()) {
                newScope.insert(bvar->name(), next);
                bvar->setBVarDepth(next++);
            }
            
            for(; it!=itEnd; ++it)
                variableDepth(*it, next, newScope, c);
            
        }    break;
        case Object::apply: {
            const Apply *c = (const Apply*) o;
            
            Object* ul=c->ulimit(), *dl=c->dlimit(), *dn=c->domain();
            
            //uplimit and downlimit are in the parent scope
            if(ul) variableDepth(ul, next, scope);
            if(dl) variableDepth(dl, next, scope);
            if(dn) variableDepth(dn, next, scope);
            
            QMap<QString, int> newScope=scope;
            
            foreach(Ci* bvar, c->bvarCi()) {
                newScope.insert(bvar->name(), next);
                bvar->setBVarDepth(next++);
            }
            
            Apply::const_iterator it = c->firstValue(), itEnd=c->constEnd();
            for(; it!=itEnd; ++it) {
                int n=next;
                variableDepth(*it, n, newScope, c);
            }
        }    break;
        case Object::none:
        case Object::value:
        case Object::custom:
        case Object::oper:
            break;
    }
}

void Expression::computeDepth(Object* o)
{
    if(o) {
        QMap<QString, int> scope;
        int next=0;
        variableDepth(o, next, scope);
    }
}

bool Expression::setMathML(const QString & s)
{
    d->m_err.clear();
    delete d->m_tree;
    
    QDomDocument doc;
    
    if (!doc.setContent(s)) {
        d->m_err << QCoreApplication::tr("Error while parsing: %1").arg(s);
        return false;
    }
    
    d->m_tree = d->branch(doc.documentElement());
    
    computeDepth(d->m_tree);
    
    return d->m_tree;
}

Object* Expression::ExpressionPrivate::branch(const QDomElement& elem)
{
    Cn *num; Operator *op;
    Object* ret=nullptr;
    
    if(elem.tagName()==QLatin1String("cs"))
        ret=listFromString(elem.text());
    else switch(whatType(elem.tagName())) {
        case Object::container: {
            Container::ContainerType t = Container::toContainerType(elem.tagName());
            
            if(t!=Container::none) {
                Container* c=addElements<Container>(new Container(t), &elem);
                
                if(c && !check(c)) {
                    delete c;
                    c=nullptr;
                }
                ret = c;
            } else
                m_err << QCoreApplication::translate("An error message", "Container unknown: %1").arg(elem.tagName());
        }    break;
        case Object::value:
            num= new Cn(0.);
            if(!num->setValue(elem)) {
                delete num;
                m_err<< QCoreApplication::tr("Cannot codify the %1 value.").arg(elem.text());
            } else
                ret = num;
            break;
        case Object::oper:
            if(elem.hasChildNodes()) {
                m_err << QCoreApplication::tr("The %1 operator cannot have child contexts.").arg(elem.tagName());
            } else {
                Operator::OperatorType type=Operator::toOperatorType(elem.tagName());
                if(type==Operator::none)
                    m_err << QCoreApplication::tr("The element '%1' is not an operator.").arg(elem.tagName());
                else
                    ret = op = new Operator(type);
            }
            break;
        case Object::variable: {
            Ci* var = new Ci(elem.text());
            var->setFunction(elem.attribute(QStringLiteral("type"))==QLatin1String("function"));
            ret=var;
        }    break;
        case Object::vector: {
            ret=addElements<Vector>(new Vector(elem.childNodes().count()), &elem);
            if(elem.childNodes().count()==0) {
                m_err << QCoreApplication::tr("Do not want empty vectors");
            }
//             qDebug() << "PEEEEEEU" << v->size();
        }    break;
        case Object::list:
            ret=addElements<List>(new List, &elem);
            break;
        case Object::matrix:
            ret=addElements<Matrix, MatrixRow>(new Matrix, &elem);
            if(elem.childNodes().count()==0) {
                m_err << QCoreApplication::tr("Do not want empty matrices");
            }
            break;
        case Object::matrixrow:
            ret=addElements<MatrixRow>(new MatrixRow, &elem);
            if(elem.childNodes().count()==0) {
                m_err << QCoreApplication::tr("Do not want empty matrixrow elements");
            }
            break;
        case Object::apply: {
            Apply *a=addElements<Apply>(new Apply, &elem);
            if(a && !check(a)) {
                delete a;
                a=nullptr;
            }
            ret=a;
        }    break;
        case Object::none:
        case Object::custom:
            m_err << QCoreApplication::translate("Error message due to an unrecognized input",
                              "Not supported/unknown: %1").arg(elem.tagName());
            break;
    }
    
    Q_ASSERT(ret || !m_err.isEmpty());
    return ret;
}

QString Expression::toHtml() const
{
    Q_ASSERT(isCorrect());
    return HtmlExpressionWriter(d->m_tree).result().toString();
}

QString Expression::toMathMLPresentation() const
{
    Q_ASSERT(isCorrect());
    return MathMLPresentationExpressionWriter(d->m_tree).result().toString();
}

QString Expression::toMathML() const
{
    Q_ASSERT(isCorrect());
    return MathMLExpressionWriter(d->m_tree).result().toString();
}

QString Expression::toString() const
{
    Q_ASSERT(isCorrect());
    StringExpressionWriter s(d->m_tree);
    return s.result().toString();
}

enum Object::ObjectType Expression::whatType(const QString& tag)
{
    Object::ObjectType ret=Object::none;
    if(tag==QLatin1String("cn"))
        ret= Object::value;
    else if(tag==QLatin1String("ci"))
        ret= Object::variable;
    else if(tag==QLatin1String("vector"))
        ret= Object::vector;
    else if(tag==QLatin1String("list"))
        ret= Object::list;
    else if(tag==QLatin1String("matrix"))
        ret= Object::matrix;
    else if(tag==QLatin1String("matrixrow"))
        ret= Object::matrixrow;
    else if(tag==QLatin1String("apply"))
        ret= Object::apply;
    else if(Operator::toOperatorType(tag)!=0)
        ret= Object::oper;
    else if(Container::toContainerType(tag)!=0)
        ret= Object::container;
    
    return ret;
}

bool Expression::operator==(const Expression & e) const
{
    return AnalitzaUtils::equalTree(e.d->m_tree, d->m_tree);
}

bool Expression::operator!=(const Expression& e) const
{
    return !(e==*this);
}

void Expression::clear()
{
    delete d->m_tree;
    d->m_tree=nullptr;
    d->m_err.clear();
}

bool Expression::isCorrect() const
{
    return !(d || d->m_tree) || d->m_err.isEmpty();
}

QStringList Expression::bvarList() const
{
    Q_ASSERT(d->m_tree);
    const Object *o = d->m_tree;
    
    if(o->isContainer() && static_cast<const Container*>(o)->containerType()==Container::math) {
        if(!static_cast<const Container*>(o)->m_params.isEmpty())
            o = (Container*) static_cast<const Container*>(o)->m_params[0];
    }
    
    if(o->isApply())
        return static_cast<const Apply*>(o)->bvarStrings();
    else if(o->isContainer())
        return static_cast<const Container*>(o)->bvarStrings();
    
    return QStringList();
}

Cn Expression::toReal() const
{
    Object* tree=d->m_tree;
    if(Q_LIKELY(tree && tree->type()==Object::value))
        return *static_cast<Cn*>(tree);
    else {
        qDebug() << "trying to return not a real value as real:" << (tree ? tree->toString() : QStringLiteral("null"));
        return Cn(0.);
    }
}

QString Expression::stringValue() const
{
    Object* tree=d->m_tree;
    if(Q_LIKELY(d->m_tree && d->m_tree->type()==Object::list))
        return AnalitzaUtils::listToString(static_cast<const List*>(tree));
    else {
        qDebug() << "trying to return not a string value as string:" << (tree ? tree->toString() : QStringLiteral("null"));
        return QString();
    }
}

bool Expression::isString() const
{
    return d->m_tree && d->m_tree->type()==Object::list
            && !static_cast<const List*>(d->m_tree)->isEmpty()
            && static_cast<const List*>(d->m_tree)->at(0)->type()==Object::value
            && static_cast<const Cn*>(static_cast<const List*>(d->m_tree)->at(0))->isCharacter();
}

//if it's a math element, then return the (only) son
Object* actualRoot(Object* o)
{
    if(o && o->isContainer()) {
        Container* c = (Container*) o;
        if(c->containerType()==Container::math) {
            return c->m_params[0];
        }
    }
    return o;
}

bool Expression::isLambda() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isContainer()) {
        Container* c = (Container*) o;
        return c->containerType()==Container::lambda;
    }
    return false;
}

bool Expression::isDeclaration() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isContainer()) {
        Container* c = (Container*) o;
        return c->containerType() == Container::declare;
    }
    return false;
}

QString Expression::name() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isContainer()) {
        Container* c = (Container*) o;
        if(c->containerType() == Container::declare)
            return static_cast<const Ci*>(c->m_params[0])->name();
    }
    return QString();
}

Expression Expression::declarationValue() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isContainer()) {
        Container* c = (Container*) o;
        if(c->containerType() == Container::declare)
            return Expression(c->m_params[1]->copy());
    }
    return Expression();
}

bool Expression::isEquation() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isApply()) {
        Apply *c1 = (Apply*) o;
        return c1->firstOperator().operatorType()==Operator::eq;
    }
    return false;
}

Expression Expression::equationToFunction() const
{
    Object* o = actualRoot(d->m_tree);
    if(o && o->isApply()) {
        Apply *c1 = (Apply*) o;
        if(c1->firstOperator().operatorType()==Operator::eq) {
            Container* c = new Container(Container::math);
            Apply* a = new Apply;
            a->appendBranch(new Operator(Operator::minus));
            a->appendBranch(c1->at(0)->copy());
            a->appendBranch(c1->at(1)->copy());
            c->appendBranch(a);
            return Expression(c);
        }
    }
    return *this;
}

QList<Ci*> Expression::parameters() const
{
    QList<Ci*> ret;
    Object* o = actualRoot(d->m_tree);
    if(o && o->isContainer()) {
        Container* c1 = (Container*) o;
        return c1->bvarCi();
    }
    return ret;
}

Expression Expression::lambdaBody() const
{
    Q_ASSERT(isLambda());
    Object* ob = actualRoot(d->m_tree);
    if(ob->isContainer()) {
        Container* c = (Container*) ob;
        Q_ASSERT(c->containerType()==Container::lambda);
        Object *ret = c->m_params.last();
        Object* o = ret->copy();
        computeDepth(o);
        return Expression(o);
    }
    return {};
}

bool Expression::isVector() const
{
    Object* o = actualRoot(d->m_tree);
    return o && o->type()==Container::vector;
}

bool Expression::isMatrix() const
{
    Object* o = actualRoot(d->m_tree);
    return o && o->type()==Container::matrix;
}

bool Expression::isList() const
{
    Object* o = actualRoot(d->m_tree);
    return o && o->type()==Container::list;
}

QList<Expression> Expression::toExpressionList() const
{
    bool isvector = isVector();
    if((!isvector && !isList()) || !d->m_tree)
        return QList<Expression>();
    
    QList<Expression> ret;
    const Object* o=actualRoot(d->m_tree);

    if(isvector) {
        const Vector* v = (const Vector*) o;
        for(Vector::const_iterator it=v->constBegin(), itEnd=v->constEnd(); it!=itEnd; ++it) {
            Object* newelement = (*it)->copy();
            computeDepth(newelement);
            ret << Expression(newelement);
        }
    } else {
        const List* v = (const List*) o;
        for(List::const_iterator it=v->constBegin(), itEnd=v->constEnd(); it!=itEnd; ++it) {
            Object* newelement = (*it)->copy();
            computeDepth(newelement);
            ret << Expression(newelement);
        }
    }
    
    return ret;
}

Expression Expression::elementAt(int position) const
{
    Q_ASSERT(isVector());
    Object* o = actualRoot(d->m_tree);
    Q_ASSERT(o);
    Vector *v=static_cast<Vector*>(o);
    
    return Expression(v->at(position)->copy());
}

void Expression::setElementAt(int position, const Expression& exp)
{
    Q_ASSERT(isVector());
    Object* o = actualRoot(d->m_tree);
    Q_ASSERT(o);
    Vector *v=static_cast<Vector*>(o);
    
    delete v->at(position);
    v->setAt(position, exp.tree()->copy());
}

QStringList Expression::error() const
{
    return d->m_err;
}

void Expression::addError(const QString& error)
{
    d->m_err += error;
}

const Object* Expression::tree() const
{
    return d->m_tree;
}

Object* Expression::tree()
{
    return d->m_tree;
}

Object* Expression::takeTree()
{
    Object* ret = d->m_tree;
    setTree(Q_NULLPTR);
    return ret;
}

void Expression::setTree(Object* o)
{
    d->m_tree=o;
}

bool Expression::isReal() const { return d->m_tree && d->m_tree->type()==Object::value; }
bool Expression::isCustomObject() const { return d->m_tree && d->m_tree->type()==Object::custom; }

Expression Expression::constructString(const QString& str)
{
    return Expression(ExpressionPrivate::listFromString(str));
}

Expression Expression::constructList(const QList< Expression >& exps)
{
    List* l = new List;
    foreach(const Expression& e, exps)
        l->appendBranch(e.tree()->copy());
    
    computeDepth(l);
    return Expression(l);
}

#define CHECK_TYPE(type)\
        type* a = (type *) o;\
        \
        type::iterator it=a->begin(), itEnd=a->end();\
        for(; it!=itEnd; ++it) {\
            renameTree(*it, depth, newName);\
        }\

static void renameTree(Object* o, int depth, const QString& newName)
{
    if(!o)
        return;
    
    switch(o->type()) {
        case Object::variable: {
            Ci* var = (Ci*) o;
            if(var->depth()==depth) 
                var->setName(newName);
        }    break;
        case Object::apply: {
            CHECK_TYPE(Apply);
            renameTree(a->ulimit(), depth, newName);
            renameTree(a->dlimit(), depth, newName);
            renameTree(a->domain(), depth, newName);
        }    break;
        case Object::container: { CHECK_TYPE(Container); } break;
        case Object::list: { CHECK_TYPE(List); } break;
        case Object::vector: { CHECK_TYPE(Vector); } break;
        case Object::matrix: { CHECK_TYPE(Matrix); } break;
        case Object::matrixrow: { CHECK_TYPE(MatrixRow); } break;
        
        case Object::none:
        case Object::value:
        case Object::oper:
        case Object::custom:
            break;
    }
}

void Expression::renameArgument(int depth, const QString& newName)
{
    renameTree(d->m_tree, depth, newName);
    computeDepth(d->m_tree);
}

Expression Expression::constructCustomObject(const QVariant& custom, CustomObjectDestructor d)
{
    Object* obj = new CustomObject(custom, d);
    return Expression(obj);
}

QVariant Expression::customObjectValue() const
{
    Object* tree=d->m_tree;
    if(Q_LIKELY(tree && tree->type()==Object::custom))
        return static_cast<const CustomObject*>(tree)->value();
    else {
        qDebug() << "trying to return as a custom object something that it is not:" << (tree ? tree->toString() : QStringLiteral("null"));
        return QVariant();
    }
}

// static void print_dom(const QDomNode& in, int ind)
// {
//     QString a;
//
//     if(ind >100){
//         qDebug("...");
//         return;
//     }
//
//     for(int i=0; i<ind; i++)
//         a.append("______|");
//
//     if(in.hasChildNodes())
//         qDebug("%s%s(%s) -- %d", qPrintable(a), qPrintable(in.toElement().tagName()), qPrintable(in.toElement().text()), in.childNodes().length());
//     else
//         qDebug("%s%s", qPrintable(a), qPrintable(in.toElement().tagName()));
//
//     for(int i=0 ; i<in.childNodes().length(); i++){
//         if(in.childNodes().item(i).isElement())
//             print_dom(in.childNodes().item(i), ind+1);
//     }
// }

bool Expression::isCompleteExpression(const QString& exp, bool justempty)
{
    ExpLexer lex(exp);
    return lex.isCompleteExpression(justempty);
}

QStringList Expression::comments() const
{
    return m_comments;
}
