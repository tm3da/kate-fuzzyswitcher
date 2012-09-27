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


#ifndef SORTFUZZYFILTERPROXYMODEL_H
#define SORTFUZZYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class QString;

class SortFuzzyFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFuzzyFilterProxyModel(QObject *parent = 0);

public slots:
    void setFuzzyFilterString(const QString &pattern);

signals:


protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    virtual bool lessThan(const QModelIndex & left, const QModelIndex & right) const;
    QString m_fuzzyfilter;
private:
    int getPoints(const QString & str) const;
public slots:

};

#endif // SORTFUZZYFILTERPROXYMODEL_H
