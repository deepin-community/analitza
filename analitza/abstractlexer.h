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

#ifndef ABSTRACTLEXER_H
#define ABSTRACTLEXER_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QQueue>
#include "analitzaexport.h"

class ANALITZA_EXPORT AbstractLexer
{
    public:
        struct TOKEN
        {
            TOKEN(int _type=-1, uint _pos=0, const QString& _val=QString(), unsigned char _len=0) :
                type(_type), val(_val), len(_len), pos(_pos) {}
            
            int type;
            QString val;
            unsigned char len;
            uint pos;
        };
        
        explicit AbstractLexer(const QString &source);
        virtual ~AbstractLexer();
        int lex();
        int lineNumber() const { return m_lines; }
        bool isCompletelyRead() const { return m_openPr==0 && m_openCb==0; }
        
        TOKEN current;
        QString error() const { return m_err; }
        bool isCompleteExpression(bool justempty=false);
        
    protected:
        QString m_err;
        QString m_source;
        
        int m_lines;
        int m_openPr, m_openCb;
        QQueue<TOKEN> m_tokens;
        static QHash<QChar, int> m_operators;
        static QHash<QString, int> m_longOperators;
        virtual void getToken()=0;
        void printQueue(const QQueue<TOKEN>& q) const;
};

#endif
