/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
    ---
    Copyright (C) 2011 Tomáš Frýda <fryda.tomas@gmail.com>
*/


#include "sortfuzzyfilterproxymodel.h"

#include <QModelIndex>
#include <QString>
#include <QStringList>


SortFuzzyFilterProxyModel::SortFuzzyFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_fuzzyfilter("")
{
}

void SortFuzzyFilterProxyModel::setFuzzyFilterString(const QString &pattern)
{
    m_fuzzyfilter = pattern;
    invalidateFilter();
    QSortFilterProxyModel::sort(0);
}

bool SortFuzzyFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QString _left = sourceModel()->data(left).toString();
    QString _right = sourceModel()->data(right).toString();

    if (m_fuzzyfilter=="")
        return QSortFilterProxyModel::lessThan(left,right);

    return getPoints(_left)<getPoints(_right);
}

int SortFuzzyFilterProxyModel::getPoints(const QString &str) const
{

    if (m_fuzzyfilter=="")
    {
        return 0;
    }
    QStringList _list;
    if (m_fuzzyfilter.indexOf("_")>=0)
    {
        _list = str.split(QRegExp("[/\\-\\\\:\\.]"));
    }
    else
    {
        _list = str.split(QRegExp("[/\\-\\\\_:\\.]"));
    }

    unsigned _index = 0;
    int _word = 0;
    unsigned _points = 0;

    foreach (QChar c, m_fuzzyfilter) {
        bool _s = true;
        while (_word<_list.count() && _s)
        {
            if (_list.at(_word).at(_index) == c)
            {
                _s=false;
                _index++;
            }
            else
            {
                _points++;
                _word++;
                _index = 0;
            }
        }
        if (_word>=_list.count() && _s)
            return 10000;
    }

    return _points;
}



bool SortFuzzyFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex _index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex _index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QString _first = sourceModel()->data(_index0).toString();
    QString _second = sourceModel()->data(_index1).toString();

    return (getPoints(_first)<3) || (getPoints(_second)<50);
}
