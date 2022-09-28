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

#ifndef CONTAINER_H
#define CONTAINER_H

#include <QDomNode>

#include "object.h"
#include "operator.h"
#include "analitzaexport.h"

namespace Analitza
{

class Ci;

/**
 * \class Container
 * 
 * \ingroup AnalitzaModule
 *
 * \brief Container represents special tags of MathML called containers.
 *
 * This class is the one that will correspond to MathML container.
 * e.g. apply, mathml, bvar, uplimit...
 */

class ANALITZA_EXPORT Container : public Object
{
public:
    
    /** Is used to describe Container objects in reference to the MathML standard*/
    enum ContainerType {
        none=0,     /**< No container type, usually means an error */
        math,        /**< Describes a container as a %lt;math&gt; tag */
        declare,    /**< Describes a container as a %lt;declare&gt; tag */
        lambda,        /**< Describes a container as a %lt;lambda&gt; tag */
        bvar,        /**< Describes a container as a %lt;bvar&gt; tag */
        uplimit,    /**< Describes a container as a %lt;uplimit&gt; tag */
        downlimit,    /**< Describes a container as a %lt;downlimit&gt; tag */
        piece,        /**< Describes a container as a %lt;piece&gt; tag */
        piecewise,    /**< Describes a container as a %lt;piecewise&gt; tag */
        otherwise,    /**< Describes a container as a %lt;otherwise&gt; tag */
        domainofapplication        /**< Describes a container as a %lt;domainofapplication&gt; tag */ 
    };
    
    typedef QList<Object*>::const_iterator const_iterator;
    typedef QList<Object*>::iterator iterator;
    
    /** Construtor. Creates an empty container with @p c type. */
    Container(enum ContainerType c) : Object(container), m_cont_type(c) { }
    
    /** Copy constructor, copies all of the branches derivated on it.*/
    Container(const Container& c);
    
    virtual Container* copy() const override;
    
    /** Destructor. Deletes all the references. */
    virtual ~Container() { qDeleteAll(m_params); }
    
    /** Sets the container type to @p c. */
    void setContainerType(enum ContainerType c) { m_cont_type = c; }
    
    /** Returns the type of the container. */
    ContainerType containerType() const { Q_ASSERT(m_type==Object::container && m_cont_type!=none); return m_cont_type; }
    
    /** Returns whether @p c is equal or not. */
    bool operator==(const Container& c) const;
    
    /** Converts a @p tag to a containerType. */
    static ContainerType toContainerType(const QString& tag);
    
    /** Adds a @p o branch at the end of the Container. */
    void appendBranch(Object* o);
    
    /** Adds a @p o branch right after @p before of the Container. */
    void insertBranch(const Container::iterator &before, Object* o) { m_params.insert(before, o); }
    
    /** Returns a QStringList where we have all of the bvar in the container */
    QStringList bvarStrings() const;
    
    /** Returns a QStringList where we have all of the bvar in the container */
    QList<Ci*> bvarCi() const;
    
    /** Returns the begin iterator on the contained object list */
    Container::iterator begin() { return m_params.begin(); }
    
    /** Returns the begin iterator on the contained object list */
    Container::const_iterator constBegin() const { return m_params.constBegin(); }
    
    /** Returns the end iterator on the contained object list */
    Container::const_iterator constEnd() const { return m_params.constEnd(); }
    
    /** Returns the end iterator on the contained object list */
    Container::iterator end() { return m_params.end(); }
    
    /** Returns whether it is an empty container. */
    bool isEmpty() const { return m_params.isEmpty(); }
    
    /** @return Returns whether it provides a numerical result instead of information. */
    bool isNumber() const;
    
    /** @return Returns the string associated to the container type. */
    QString tagName() const;
    
    virtual QVariant accept(AbstractExpressionVisitor*) const override;
    
    virtual bool isZero() const override;
    
    virtual bool matches(const Object* pattern, QMap< QString, const Object* >* found) const override;
    
    const Container* extractType(Container::ContainerType t) const;
    
    /** @returns how many bvars are there in the container */
    int bvarCount() const;

// protected:
    QList<Object*> m_params;
private:
    ContainerType m_cont_type;
    static char m_typeStr[][20];
    static QMap<QString, ContainerType> m_nameToType;
};

}

#endif
