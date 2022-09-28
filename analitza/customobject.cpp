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

#include "customobject.h"
#include "abstractexpressionvisitor.h"

using namespace Analitza;

CustomObject::CustomObject(const QVariant& v, CustomObject::destructor f, int* r)
    : Object(Object::custom), m_destructor(f), m_refcount(r), m_value(v)
{}

CustomObject::CustomObject(const QVariant& v, CustomObject::destructor f)
    : Object(Object::custom), m_destructor(f), m_refcount(new int(1)), m_value(v)
{}

CustomObject::~CustomObject()
{
    (*m_refcount)--;
    if(*m_refcount==0) {
        delete m_refcount;
        
        if(m_destructor)
            m_destructor(m_value);
    }
}

Object* CustomObject::copy() const
{
    (*m_refcount)++;
    return new CustomObject(m_value, m_destructor, m_refcount);
}

bool CustomObject::matches(const Analitza::Object* exp, QMap< QString, const Analitza::Object* >* ) const
{
    bool ret = exp->type()==type();
    ret &= *this==*static_cast<const CustomObject*>(exp);
    return ret;
}

bool CustomObject::operator==(const CustomObject& obj) const
{
    return this==&obj || obj.m_value == m_value;
}

QVariant CustomObject::accept(AbstractExpressionVisitor* exp) const
{
    return exp->visit(this);
}
