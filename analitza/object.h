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

#ifndef OBJECT_H
#define OBJECT_H

#include <QDebug>

#include "analitzaexport.h"

namespace Analitza
{

class AbstractExpressionVisitor;

/**
 *    \internal
 *    Abstract expression tree node
 *    @author Aleix Pol <aleixpol@kde.org>  
 */

//FIXME: Check for public -> protected on some members
class ANALITZA_EXPORT Object
{
    Q_GADGET
public:
    /** ObjectType is used to describe objects. */
    enum ObjectType {
        none=0,        /**< No object type, usually means an error. */
        value,        /**< Describes an object as a value. */
        variable,    /**< Describes an object as a variable. */
        vector,        /**< Describes an object as a vector. */
        list,        /**< Describes an object as a list. */
        apply,        /**< Describes an object as an application. */
        oper,        /**< Describes an object as an operator. */
        container,    /**< Describes an object as a container. */
        matrix,        /**< Describes an object as a matrix. */
        matrixrow,    /**< Describes an object as a matrix row. */
        custom        /**< Describes a custom object */
    };
    Q_ENUM(ObjectType);
    
    /** Object destructor. Does nothing. */
    virtual ~Object() { /*qDebug() << "Destroying " << this;*/}
    
    /** Returns the object type of the object */
    enum ObjectType type() const { return m_type; }
    
    /** Returns whether it is an apply or not. */
    bool isApply() const { return m_type==apply; }
    
    /** Returns whether it is a container or not. */
    bool isContainer() const { return m_type==container; }
    
    /** Returns whether it is a none object. Useful for checking for errors */
    bool isNone() const { return m_type==none; }

    /** Returns the string representation of the object. */
    QString toString() const;
    
    /** Returns some string depending on the visior */
    virtual QVariant accept(AbstractExpressionVisitor* exp) const = 0;
    
    virtual bool isZero() const { return false; }
    
    /** 
        @p exp is the tree that we will compare to,
        @p found is where we will pass the variables store the results.
        
        It will return whether the object follows the @p pattern structure.
    */
    virtual bool matches(const Object* exp, QMap<QString, const Object*>* found) const=0;
    
    /** @returns a new and equal instance of the tree. */
    virtual Object* copy() const =0;
    
protected:
    /** Creates an object with a @p t type */
    Object(enum ObjectType t) : m_type(t) {}
    
    /** Specifies the Object type. */
    const ObjectType m_type;
};


/**
 * \class None
 * 
 * \ingroup AnalitzaModule
 *
 * \brief A empty object that indicates usually an error in the expression tree. 
 * 
 * The default convention for an error value is None and the default text to print is 
 * an empty string. This class does not hold an error string, since any error string will 
 * be manage mainly in Analyzer, Expression and ExpressionTypeChecker. This class is just a 
 * flag in memory to indicate that something went wrong in the expression tree.
 */

class ANALITZA_EXPORT None : public Object
{
    public:
        explicit None() : Object(Object::none) {}
        virtual ~None() {}
        
        bool operator==(const None& ) const { return true; }
        
        QString toMathML() const { return QString(); }
        QString toHtml() const { return QString(); }
        virtual QVariant accept(Analitza::AbstractExpressionVisitor* visitor) const override;
        virtual bool matches(const Object* pattern, QMap<QString, const Object*>* found) const override;
        None* copy() const override;
};

}
#endif
