/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef BUILTINMETHODS_H
#define BUILTINMETHODS_H
#include <QVariant>
#include "analitzaexport.h"
#include "expressiontype.h"
#include <QStringList>

namespace Analitza
{

class Expression;
class ExpressionType;

class FunctionDefinition
{
    public:
        virtual ~FunctionDefinition() {}
        
        /** 
         * Lets the user specify a function to be injected. When called it should perform whatever
         * the function is supposed to do.
         * 
         * @param args: specifies the values passed as arguments.
         * @returns the resulting expression.
         */
        virtual Expression operator()(const QList<Expression>& args)=0;
};

class PointerFunctionDefinition : public FunctionDefinition
{
    public:
        typedef Expression (*func)(const QList<Expression>& args);
        
        explicit PointerFunctionDefinition(func call);
        virtual Expression operator()(const QList<Expression>& args) override;
    private:
        func m_function;
};

/**
 * \class BuiltinMethods
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Manage custom commands in analitza language.
 */

class ANALITZA_EXPORT BuiltinMethods
{
    public:
        ~BuiltinMethods();
        
        //TODO improve doc of variadic function ... and put some examples ...
        //TODO unit test of variadic function with ExpressionType/Checker 
        /**
            Adds a new function to the system identified @p id with @p type that can be called using @p f
            For variadic function use ExpressionType::Any as first lambda parameter. Also, if you use ExpressionType::Any
            with some type as contained, then it means the all arguments of variadic function must be of the same type, equals 
            to the contained type.
        */
        void insertFunction(const QString& id, const ExpressionType& type, FunctionDefinition* f);
        
        /** @returns whether it exists a builtin function named like @p id */
        bool contains(const QString& id) const { return m_functions.contains(id); }
        
        /** @returns a map that relates all functions with their specified type. */
        QMap<QString, ExpressionType> varTypes() const { return m_types; }
        
        /** @returns the builtin function identified by @p id to be called. */
        FunctionDefinition* function(const QString& id) const { return m_functions.value(id); }
        
        /** @returns a list with the name of all registered identifiers. */
        QStringList identifiers() const { return m_functions.keys(); }
    private:
        QMap<QString, ExpressionType> m_types;
        QHash<QString, FunctionDefinition*> m_functions;
};

}

#endif
