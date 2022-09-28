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

#include "explexer.h"
#include "expressionparser.h"

#include <QDebug>
#include <QCoreApplication>

ExpLexer::ExpLexer(const QString &source)
    : AbstractLexer(source), m_pos(0)
    , m_realRx("^-?((\\.[0-9]+)|[0-9]+(\\.[0-9]+)?)(e-?[0-9]+)?", Qt::CaseSensitive, QRegExp::RegExp2)
{}

QString ExpLexer::escape(const QString& str)
{
    QString ret=str;
    ret.replace('&', QLatin1String("&amp;"));
    ret.replace('<', QLatin1String("&lt;"));
    ret.replace('>', QLatin1String("&gt;"));
    ret.replace('\'', QLatin1String("&apos;"));
    ret.replace('"', QLatin1String("&quot;"));
    return ret;
}

void ExpLexer::getToken()
{
    int& pos=m_pos;
    const QString& a=m_source;
    for(; pos<a.length() && a[pos].isSpace(); pos++) {}
    
    int oldpos=pos;
    TOKEN ret(-1, pos);
    
    if(pos>=a.length()) {
        ret.type = ExpressionTable::EOF_SYMBOL;
    } else if(a.length()>pos+1 && a[pos]=='/' && a[pos+1]=='/') {
        ret.type=ExpressionTable::tComment;
        pos+=2;
        for(; a.length()>pos; pos++) {
            if((a.length()>pos+1 && a[pos]=='/' && a[pos+1]=='/') || a[pos]=='\n') {
                pos+= a[pos]=='\n' ? 1 : 2;
                break;
            }
        }
        ret.val=a.mid(oldpos, pos-oldpos);
    } else if(a.length()>pos+1 && m_longOperators.contains(a.mid(pos, 2))) {
        ret.type=m_longOperators[a.mid(pos, 2)];
        pos+=2;
    } else if(m_operators.contains(a[pos])) {
        ret.type=m_operators[a[pos]];
        pos++;
    } else if(a[pos]=='"') {
        bool escaping=false;
        pos++;
        int posini=pos;
        
        for(; pos<a.size() && (a[pos]!='"' || escaping); pos++)
            escaping=a[pos]=='\\';
        
        if(pos>=a.size())
            m_err += QCoreApplication::tr("Unexpectedly arrived to the end of the input");
        
        ret.type=ExpressionTable::tString;
        ret.val="<cs>"+escape(a.mid(posini, pos-posini))+"</cs>";
        pos++;
    } else if(a[pos].decompositionTag()==QChar::Super) {
        QString super;
        for(int i=pos; i<a.count() && a[i].decompositionTag()==QChar::Super; i++) {
            super+=a[i].decomposition()[0];
        }
        pos+=super.size();
        ret.type = ExpressionTable::tUniPow;
        ret.val = "<cn>"+super+"</cn>";
    } else if(a[pos].isLetter()) {
        for(; pos<a.length() && (a[pos]=='_' || a[pos].isLetter() || (a[pos].isNumber() && a[pos].decompositionTag()==QChar::NoDecomposition)); pos++){
            ret.val += a[pos];
        }
        ret.type= ExpressionTable::tId;
        Q_ASSERT(!ret.val.isEmpty());
    } else if(m_realRx.indexIn(a, pos, QRegExp::CaretAtOffset)==pos) {
        ret.val = m_realRx.cap();
        
        QString attrib;
        if(!m_realRx.cap(2).isEmpty() || !m_realRx.cap(3).isEmpty() || !m_realRx.cap(4).isEmpty())
            attrib+=QLatin1String(" type='real'");
        
        Q_ASSERT(!ret.val.isEmpty());
        
        ret.val = QStringLiteral("<cn%1>%2</cn>").arg(attrib, ret.val);
        ret.type= ExpressionTable::tVal;
        
        pos += m_realRx.matchedLength();
    } else {
        ret.val=-1;
        m_err=QCoreApplication::tr("Unknown token %1").arg(a[pos]);
    }
    ret.len = pos-oldpos;
    m_tokens.append(ret);
    
//     printQueue(m_tokens);
}
