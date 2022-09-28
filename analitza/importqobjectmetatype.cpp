/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "importqobjectmetatype.h"
#include "analyzer.h"
#include <QMetaProperty>
#include "analitzautils.h"
#include "value.h"

using namespace Analitza;

ExpressionType toExpressionType(QVariant::Type t, const QString& type_name)
{
    switch(t)
    {
        case QVariant::Int:
        case QVariant::Double:
            return ExpressionType(ExpressionType::Value);
        case QVariant::String:
            return ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Char));
        default:
            return ExpressionType(type_name);
    }
}

class QObjectGet : public Analitza::FunctionDefinition
{
    public:
        QObjectGet(const QByteArray& name) : m_name(name) {}
    
        virtual Expression operator()(const QList< Expression >& args) override
        {
            QObject* o=args.first().customObjectValue().value<QObject*>();
            QVariant v=o->property(m_name);
            
            return AnalitzaUtils::variantToExpression(v);
        }
        
        ExpressionType type(const QByteArray& name, const QMetaProperty& prop) const
        {
            ExpressionType typeGet(ExpressionType::Lambda);
            typeGet.addParameter(ExpressionType(name))
                .addParameter(toExpressionType(prop.type(), prop.typeName()));
            
            return typeGet;
        }
        
    private:
        QByteArray m_name;
};

class QObjectSet : public Analitza::FunctionDefinition
{
    public:
        QObjectSet(const QByteArray& name) : m_name(name) {}
    
        virtual Expression operator()(const QList< Expression >& args) override
        {
            QObject* o=args.first().customObjectValue().value<QObject*>();
            QVariant value=AnalitzaUtils::expressionToVariant(args.last());
            
            return Expression(Cn(o->setProperty(m_name, value)));
        }
        
        ExpressionType type(const QByteArray& name, const QMetaProperty& prop) const
        {
            ExpressionType typeSet(ExpressionType::Lambda);
            typeSet.addParameter(ExpressionType(name))
                .addParameter(toExpressionType(prop.type(), prop.typeName()))
                .addParameter(ExpressionType(ExpressionType::Bool));
            
            return typeSet;
        }
        
    private:
        QByteArray m_name;
};

class QObjectCastToParent : public Analitza::FunctionDefinition
{
    public:
        QObjectCastToParent(const QByteArray& name, const QByteArray& nameParent)
            : m_name(name), m_nameParent(nameParent) {}
        
        virtual Expression operator()(const QList< Expression >& args) override
        {
            QObject* o=args.first().customObjectValue().value<QObject*>();
            return Expression::constructCustomObject(QVariant::fromValue<QObject*>(o), nullptr);
        }
        
        ExpressionType type() const
        {
            ExpressionType typeCast(ExpressionType::Lambda);
            typeCast.addParameter(ExpressionType(m_name))
                .addParameter(ExpressionType(m_nameParent));
            
            return typeCast;
        }
    private:
        QByteArray m_name;
        QByteArray m_nameParent;
};

ImportQMetaObject::ImportQMetaObject(Analitza::Analyzer* a)
    : m_a(a)
{}

void ImportQMetaObject::import(const QMetaObject& t)
{
    Analitza::BuiltinMethods* b=m_a->builtinMethods();
    QByteArray classname(t.className());
    classname.replace("::", "_");
    
    for(int p=0; p<t.propertyCount(); p++) {
        QMetaProperty prop=t.property(p);
        QByteArray name(prop.name());
        
        if(prop.isReadable()) {
            QObjectGet* getter=new QObjectGet(name);
            b->insertFunction(QString(classname+'_'+name), getter->type(classname, prop), getter);
        }
        
        if(prop.isWritable()) {
            QObjectSet* setter=new QObjectSet(name);
            b->insertFunction(QString(classname+"_set_"+name), setter->type(classname, prop), setter);
        }
    }
    
    if(t.superClass()) {
        QObjectCastToParent *cast = new QObjectCastToParent(t.className(), t.superClass()->className());
        b->insertFunction(classname+QStringLiteral("_toParent"), cast->type(), cast);
    }
}

