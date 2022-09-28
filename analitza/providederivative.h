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

#ifndef PROVIDEDERIVATIVE_H
#define PROVIDEDERIVATIVE_H
#include "abstractexpressiontransformer.h"
#include <QList>
#include <QPair>
#include <QStringList>

namespace Analitza
{

class ProvideDerivative : public AbstractExpressionTransformer
{
    public:
        explicit ProvideDerivative(const QString& var);
        Object* run(const Object* o);
        
        bool isCorrect() const { return m_errors.isEmpty(); }
        QStringList errors() const { return m_errors; }
        
    private:
        QStringList m_errors;
        virtual Object* walkApply(const Analitza::Apply* pattern) override;
        
        Object* derivateContentVector(const Vector* v);
        Object* derivateContentList(const List* v);
        Object* derivativeContainer(const Container *c);
        Object* derivativeApply(const Apply *c);
        
        Apply* makeDiff(Analitza::Object* o) const;
        void clearDiff(Apply* a);
        QString var;
};

}
#endif // PROVIDEDERIVATIVE_H
