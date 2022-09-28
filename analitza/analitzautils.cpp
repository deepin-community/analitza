/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "analitzautils.h"

#include "abstractexpressionvisitor.h"
#include "vector.h"
#include "value.h"
#include "list.h"
#include "variable.h"
#include "container.h"
#include "variables.h"
#include "expression.h"
#include "apply.h"
#include <QVariant>
#include "customobject.h"
#include "matrix.h"

using namespace Analitza;
namespace AnalitzaUtils
{
    
QStringList dependencies(const Object* o, const QStringList& scope)
{
    Q_ASSERT(o);
    
    QSet<QString> ret;
    switch(o->type()) {
        case Object::variable: {
            Ci *i = (Ci*) o;
            if(!scope.contains(i->name()))
                ret += i->name();
        }    break;
        case Object::matrix: {
            const Matrix *v=(const Matrix*) o;
            for(Matrix::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                ret += dependencies(*it, scope).toSet();
            }
        }    break;
        case Object::vector: {
            const Vector *v=(const Vector*) o;
            for(Vector::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                ret += dependencies(*it, scope).toSet();
            }
        }    break;
        case Object::list: {
            const List *v=(const List*) o;
            for(List::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                ret += dependencies(*it, scope).toSet();
            }
        }    break;
        case Object::matrixrow: {
            const MatrixRow *v=(const MatrixRow*) o;
            for(MatrixRow::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                ret += dependencies(*it, scope).toSet();
            }
        }    break;
        case Object::container: {
            const Container *c = static_cast<const Container*>(o);
            int skip=c->bvarCount();
            QStringList newScope=scope+c->bvarStrings();
            if(c->containerType()==Container::declare) {
                newScope.append(static_cast<const Ci*>(*c->constBegin())->name());
                skip++;
            }
            
            for(Container::const_iterator it=c->constBegin()+skip, itEnd=c->constEnd(); it!=itEnd; ++it) {
                ret += dependencies(*it, newScope).toSet();
            }
        } break;
        case Object::apply: {
            const Apply* c = static_cast<const Apply*>(o);
            Apply::const_iterator it = c->firstValue()/*, first = c->firstValue()*/;
            
            Object* ul=c->ulimit(), *dl=c->dlimit(), *domain=c->domain();
            
            //uplimit and downlimit are in the parent scope
            if(ul) ret += dependencies(ul, scope).toSet();
            if(dl) ret += dependencies(dl, scope).toSet();
            if(domain) ret += dependencies(domain, scope).toSet();
            
            if(c->firstOperator()==Operator::function) {
                ret += dependencies(c->m_params[0], scope).toSet();
            }
            
            QStringList newScope=scope+c->bvarStrings();
            for(; it!=c->constEnd(); ++it) {
                ret += dependencies(*it, newScope).toSet();
            }
        }    break;
        case Object::none:
        case Object::custom:
        case Object::value:
        case Object::oper:
            break;
    }
    
    return ret.values();
}

bool hasTheVar(const QSet<QString> & vars, const Object * o)
{
    if(!o)
        return false;
    
    bool found=false;
    const Ci* cand;
    switch(o->type()) {
        case Object::vector: {
            const Vector *v=static_cast<const Vector*>(o);
            Vector::const_iterator it, itEnd=v->constEnd();
            for(it=v->constBegin(); it!=itEnd; ++it) {
                found |= hasTheVar(vars, *it);
            }
        }    break;
        case Object::matrix: {
            const Matrix *v=static_cast<const Matrix*>(o);
            Matrix::const_iterator it, itEnd=v->constEnd();
            for(it=v->constBegin(); it!=itEnd; ++it) {
                found |= hasTheVar(vars, *it);
            }
        }    break;
        case Object::matrixrow: {
            const MatrixRow *v=static_cast<const MatrixRow*>(o);
            MatrixRow::const_iterator it, itEnd=v->constEnd();
            for(it=v->constBegin(); it!=itEnd; ++it) {
                found |= hasTheVar(vars, *it);
            }
        }    break;
        case Object::list: {
            const List *v=static_cast<const List*>(o);
            List::const_iterator it, itEnd=v->constEnd();
            for(it=v->constBegin(); it!=itEnd; ++it) {
                found |= hasTheVar(vars, *it);
            }
        }    break;
        case Object::container: {
            const Container *c=static_cast<const Container*>(o);
            QSet<QString> bvars=c->bvarStrings().toSet(), varsCopy=vars;
            foreach(const QString &var, bvars) {
                varsCopy.remove(var);
            }
            found=hasTheVar(varsCopy, c);
        }    break;
        case Object::apply: {
            const Apply *c=static_cast<const Apply*>(o);
            QSet<QString> bvars=c->bvarStrings().toSet(), varsCopy=vars;
            foreach(const QString &var, bvars) {
                varsCopy.remove(var);
            }
            found=hasTheVar(varsCopy, c);
        }    break;
        case Object::variable:
            cand=static_cast<const Ci*>(o);
            found=vars.contains(cand->name());
            break;
        case Object::value:
        case Object::custom:
        case Object::oper:
        case Object::none:
            found=false;
            break;
    }
    return found;
}

bool hasTheVar(const QSet<QString> & vars, const Container* c)
{
    bool found=false;
    if(c->containerType()!=Container::bvar) {
        Container::const_iterator it=c->constBegin(), itEnd=c->constEnd();
        for(; !found && it!=itEnd; ++it) {
            found=hasTheVar(vars, *it);
        }
    }
    return found;
}

bool hasTheVar(const QSet<QString> & vars, const Apply* a)
{
    bool found=hasTheVar(vars, a->ulimit()) || hasTheVar(vars, a->dlimit()) || hasTheVar(vars, a->domain());
    Apply::const_iterator it=a->firstValue(), itEnd=a->constEnd();
    for(; !found && it!=itEnd; ++it) {
        found=hasTheVar(vars, *it);
    }
    return found;
}

bool isLambda(const Object* o)
{
    return o->isContainer() && static_cast<const Container*>(o)->containerType()==Container::lambda;
}

bool hasVars(const Analitza::Object* o, const QStringList& bvars)
{
    Q_ASSERT(o);
    
    bool r=false;
    switch(o->type()) {
        case Object::variable: {
            Ci *i = (Ci*) o;
            r=!bvars.contains(i->name());
            
        }    break;
        case Object::vector: {
            Vector *v=(Vector*) o;
            for(Vector::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                r |= hasVars(*it, bvars);
            }
        }    break;
        case Object::matrix: {
            Matrix *v=(Matrix*) o;
            for(Matrix::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                r |= hasVars(*it, bvars);
            }
        }    break;
        case Object::matrixrow: {
            MatrixRow *v=(MatrixRow*) o;
            for(MatrixRow::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                r |= hasVars(*it, bvars);
            }
        }    break;
        case Object::list: {
            List *v=(List*) o;
            for(List::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it) {
                r |= hasVars(*it, bvars);
            }
        }    break;
        case Object::container: {
            const Container *c = (const Container*) o;
            
            QStringList newScope=bvars+c->bvarStrings();
            Container::const_iterator it=c->m_params.constBegin(), itEnd=c->m_params.constEnd();
            if(c->containerType()==Container::declare) {
                newScope += static_cast<const Ci*>(*c->constBegin())->name();
                ++it;
            }
            
            for(; it!=itEnd; ++it) {
                r |= hasVars(*it, newScope);
            }
        }    break;
        case Object::apply: {
            const Apply *c = (const Apply*) o;
            
            const QStringList scope=bvars+c->bvarStrings();
            Object* ul=c->ulimit(), *dl=c->dlimit(), *dn=c->domain();
            
            //uplimit and downlimit are in the parent scope
            if(ul) r |= hasVars(ul, bvars);
            if(dl) r |= hasVars(dl, bvars);
            if(dn) r |= hasVars(dn, bvars);
            
            Apply::const_iterator it = c->firstValue();
            for(; !r && it!=c->constEnd(); ++it) {
                r |= hasVars(*it, scope);
            }
        }    break;
        case Object::none:
        case Object::value:
        case Object::oper:
        case Object::custom:
            r=false;
    }
    return r;
}

struct ObjectWalker : public AbstractExpressionVisitor
{
    ObjectWalker(const QByteArray& pref) : ind(0), m_prefix(pref) {}
    
    virtual QVariant visit(const None* var) override
    { qDebug() << prefix().constData() << "| none: " << var->toString(); return QString(); }
    
    virtual QVariant visit(const Operator* root) override
    { qDebug() << prefix().constData() << "| operator: " << root->toString(); return QString(); }
    
    virtual QVariant visit(const Ci* var) override
     {
        QString value;
        if(var->depth()>=0)
            value="stack("+QString::number(var->depth())+')';
        else
            value=QStringLiteral("symbols");
        
        qDebug() << prefix().constData() << "| variable: " << var->name() << "depth:" << var->depth() << "Val:" << value;
        return QString();
    }
    
    virtual QVariant visit(const Cn* num) override
    { qDebug() << prefix().constData() << "| num: " << num->value() << " format: " << num->format(); return QString(); }
    
    virtual QVariant visit(const CustomObject* c) override
    { qDebug() << prefix().constData() << "| custom " << c; return QString(); }
    
    
    virtual QVariant visit(const Container* c) override
    {
        qDebug() << prefix().constData() << "| cont: " << c->tagName();// << "=" << c->toString();
        ind++;
        for(Container::const_iterator it=c->m_params.constBegin(); it<c->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    virtual QVariant visit(const Apply* c) override
    {
        qDebug() << prefix().constData() << "| apply op:" << c->firstOperator().toString();
        ind++;
        if(c->ulimit()) { qDebug() << prefix().constData() << "ul: "; visitNow(c->ulimit()); }
        if(c->dlimit()) { qDebug() << prefix().constData() << "dl: "; visitNow(c->dlimit()); }
        if(!c->bvarCi().isEmpty()) { qDebug() << prefix().constData() << "bvars: " << c->bvarStrings(); }
        
        for(Apply::const_iterator it=c->m_params.constBegin(); it<c->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    virtual QVariant visit(const Vector* v) override
    {
        qDebug() << prefix().constData() << "| vector: " << v->size();
        ind++;
        for(Vector::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    virtual QVariant visit(const List* v) override
    {
        qDebug() << prefix().constData() << "| list: " << v->size();
        ind++;
        for(List::const_iterator it=v->constBegin(); it!=v->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    virtual QVariant visit(const Matrix* m) override {
        qDebug() << prefix().constData() << "| matrix: ";
        ind++;
        for(Matrix::const_iterator it=m->constBegin(); it!=m->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    virtual QVariant visit(const MatrixRow* m) override {
        qDebug() << prefix().constData() << "| matrix: ";
        ind++;
        for(MatrixRow::const_iterator it=m->constBegin(); it!=m->constEnd(); ++it)
            visitNow(*it);
        ind--;
        return QString();
    }
    
    QByteArray prefix()
    {
        QByteArray ret(m_prefix);
        for(int i=0; i<ind; i++)
            ret += " |_____";
        return ret;
    }
    
    void visitNow(const Object* o) { if(o) o->accept(this); else qDebug() << prefix().constData() << "Null" ;}
    
    QVariant result() const override { return QString(); }
    
    int ind;
    QByteArray m_prefix;
};

void objectWalker(const Analitza::Expression& o, const QByteArray& prefix)
{
    objectWalker(o.tree(), prefix);
}

void objectWalker(const Object* root, const QByteArray& prefix)
{
    ObjectWalker o(prefix);
    o.visitNow(root);
    
    qDebug() << prefix.constData() << ';';
}

bool equalTree(const Object * o1, const Object * o2)
{
    Q_ASSERT((o1 && o2) || (!o1 && !o2));
    if(o1==o2)
        return true;
    else if(o1->type()!=o2->type())
        return false;
    
    bool eq = false;
    switch(o2->type()) {
        case Object::variable:
            eq = *static_cast<const Ci*>(o1)==*static_cast<const Ci*>(o2);
            break;
        case Object::value:
            eq = *static_cast<const Cn*>(o1)==*static_cast<const Cn*>(o2);
            break;
        case Object::container:
            eq = *static_cast<const Container*>(o1)==*static_cast<const Container*>(o2);
            break;
        case Object::oper:
            eq = *static_cast<const Operator*>(o1)==*static_cast<const Operator*>(o2);
            break;
        case Object::vector:
            eq = *static_cast<const Vector*>(o1)==*static_cast<const Vector*>(o2);
            break;
        case Object::list:
            eq = *static_cast<const List*>(o1)==*static_cast<const List*>(o2);
            break;
        case Object::apply:
            eq = *static_cast<const Apply*>(o1)==*static_cast<const Apply*>(o2);
            break;
        case Object::custom:
            eq = *static_cast<const CustomObject*>(o1)==*static_cast<const CustomObject*>(o2);
            break;
        case Object::matrix:
            eq = *static_cast<const Matrix*>(o1)==*static_cast<const Matrix*>(o2);
            break;
        case Object::matrixrow:
            eq = *static_cast<const MatrixRow*>(o1)==*static_cast<const MatrixRow*>(o2);
            break;
        case Object::none:
            eq=false;
            Q_ASSERT(false && "Should not get here");
            break;
    }
    return eq;
}

QVariant expressionToVariant(const Analitza::Expression& res)
{
    QVariant ret;
    if(res.isString()) {
        ret = res.stringValue();
    } else if(res.isVector() || res.isList()) {
        QVariantList vals;
        
        QList<Analitza::Expression> expressions = res.toExpressionList();
        foreach(const Analitza::Expression& exp, expressions) {
            vals << expressionToVariant(exp);
        }
        
        ret = vals;
    } else if(res.isReal()) {
        Analitza::Cn val = res.toReal();
        switch(val.format()) {
            case Analitza::Cn::Boolean:
                ret = val.isTrue();
                break;
            case Analitza::Cn::Integer:
                ret = int(val.value());
                break;
            case Analitza::Cn::Char:
                ret = val.character();
                break;
            case Analitza::Cn::Complex: //TODO: figure out complex numbers on QVariant
            case Analitza::Cn::Real:
                ret = val.value();
                break;
        }
    } else
        ret = res.toString();
    
    return ret;
}

Analitza::Expression variantToExpression(const QVariant& v)
{
    if(v.type() == QVariant::String)
        return Analitza::Expression(v.toString());
    else if(v.canConvert(QVariant::Double))
        return Analitza::Expression(Analitza::Cn(v.toReal()));
    else if(v.canConvert(QVariant::List)) {
        QVariantList list = v.toList();
        QList<Analitza::Expression> expressionList;
        
        foreach(const QVariant& elem, list) {
            expressionList << variantToExpression(elem);
        }
        
        return Analitza::Expression::constructList(expressionList);
    } else if(v.canConvert<QObject*>()) {
        return Analitza::Expression::constructCustomObject(v, nullptr);
    }
    
    Q_ASSERT(false && "couldn't figure out the type");
    return Analitza::Expression();
}

QString listToString(const List* list)
{
    QString ret;
    for(List::const_iterator it=list->constBegin(), itEnd=list->constEnd(); it!=itEnd; ++it)
        ret += static_cast<const Cn*>(*it)->character();
    return ret;
}

template<class T, class Tit, class Tcontained = Object>
T* replaceDepthTemplate(int depth, T* tree, Object* towhat)
{
    Tit it=tree->begin(), itEnd=tree->end();
    for(; it!=itEnd; ++it)
        *it = static_cast<Tcontained*>(replaceDepth(depth, *it, towhat));
    return tree;
}

Object* replaceDepth(int depth, Object* tree, Object* towhat)
{
    if(!tree)
        return nullptr;
    
    Q_ASSERT(depth>=0);
    switch(tree->type()) {
        case Object::value:
        case Object::custom:
        case Object::none:
        case Object::oper:
            break;
        case Object::list:
            return replaceDepthTemplate<List, List::iterator>(depth, static_cast<List*>(tree), towhat);
        case Object::vector:
            return replaceDepthTemplate<Vector, Vector::iterator>(depth, static_cast<Vector*>(tree), towhat);
        case Object::matrix:
            return replaceDepthTemplate<Matrix, Matrix::iterator, MatrixRow>(depth, static_cast<Matrix*>(tree), towhat);
        case Object::matrixrow:
            return replaceDepthTemplate<MatrixRow, MatrixRow::iterator>(depth, static_cast<MatrixRow*>(tree), towhat);
        case Object::container:
            return replaceDepthTemplate<Container, Container::iterator>(depth, static_cast<Container*>(tree), towhat);
        case Object::variable: {
            Ci* var=(Ci*) tree;
            if(var->depth()==depth) {
                delete tree;
                tree=towhat->copy();
            }
        }    break;
        case Object::apply: {
            Apply* a=static_cast<Apply*>(tree);
            Apply::iterator it=a->firstValue(), itEnd=a->end();
            for(; it!=itEnd; ++it)
                *it=replaceDepth(depth, *it, towhat);
            
            a->domain()=replaceDepth(depth, a->domain(), towhat);
            a->dlimit()=replaceDepth(depth, a->ulimit(), towhat);
            a->ulimit()=replaceDepth(depth, a->dlimit(), towhat);
            return a;
        }    break;
    }
    return tree;
}

template<class T, class Tit>
int countDepthTemplate(int depth, const T* tree)
{
    int ret=0;
    Tit it=tree->constBegin(), itEnd=tree->constEnd();
    for(; it!=itEnd; ++it)
        ret += countDepth(depth, *it);
    return ret;
}

int countDepth(int depth, const Object* tree)
{
    if(!tree)
        return 0;
    
    Q_ASSERT(depth>=0);
    switch(tree->type()) {
        case Object::value:
        case Object::custom:
        case Object::none:
        case Object::oper:
            break;
        case Object::list:
            return countDepthTemplate<List, List::const_iterator>(depth, static_cast<const List*>(tree));
        case Object::vector:
            return countDepthTemplate<Vector, Vector::const_iterator>(depth, static_cast<const Vector*>(tree));
        case Object::matrix:
            return countDepthTemplate<Matrix, Matrix::const_iterator>(depth, static_cast<const Matrix*>(tree));
        case Object::matrixrow:
            return countDepthTemplate<MatrixRow, MatrixRow::const_iterator>(depth, static_cast<const MatrixRow*>(tree));
        case Object::container:
            return countDepthTemplate<Container, Container::const_iterator>(depth, static_cast<const Container*>(tree));
        case Object::variable: {
            Ci* var=(Ci*) tree;
            return var->depth()==depth;
        }    break;
        case Object::apply: {
            const Apply* a=static_cast<const Apply*>(tree);
            Apply::const_iterator it=a->firstValue(), itEnd=a->constEnd();
            int ret=0;
            for(; it!=itEnd; ++it)
                ret+=countDepth(depth, *it);
            
            ret+=countDepth(depth, a->domain());
            ret+=countDepth(depth, a->ulimit());
            ret+=countDepth(depth, a->dlimit());
            return ret;
        }    break;
    }
    return 0;
}

QString generateDependencyGraph(const Variables* v)
{
    QStringList special=QStringList() << QStringLiteral("check");
    QString ret;
    ret += QLatin1String("digraph G {\n");
    
    foreach(const QString& n, special) {
        ret += '\t'+n+" [shape=doublecircle];\n";
    }
    ret += '\n';
    
    for(Variables::const_iterator it=v->constBegin(), itEnd=v->constEnd(); it!=itEnd; ++it) {
        QString current = it.key();
        QStringList deps = dependencies(it.value(), QStringList());
        
        foreach(const QString& d, deps) {
            const Object* o=v->value(d);
            if(o && isLambda(o)) {
                ret += '\t'+current+" -> "+d+";\n";
            }
        }
    }
    
    ret += QLatin1String("}\n");
    return ret;
}

}
