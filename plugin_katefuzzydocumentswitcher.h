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
    Copyright (C) 2007,2009 Joseph Wenninger <jowenn@kde.org>
    Copyright (C) 2011 Tomáš Frýda <fryda.tomas@gmail.com>
*/


#ifndef _KateFuzzyDocumentSwitcher_h_
#define _KateFuzzyDocumentSwitcher_h_

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>

#include <kxmlguiclient.h>
#include <kdialog.h>

#include <qsortfilterproxymodel.h>

class QListView;
class QTreeView;
class KLineEdit;
class SortFuzzyFilterProxyModel;

namespace KTextEditor {
    class Document;
}

class PluginKateFuzzyDocumentSwitcher : public Kate::Plugin {
  Q_OBJECT
  public:
    explicit PluginKateFuzzyDocumentSwitcher( QObject* parent = 0, const QList<QVariant>& = QList<QVariant>());
    virtual ~PluginKateFuzzyDocumentSwitcher();
    virtual Kate::PluginView *createView (Kate::MainWindow *mainWindow);
};

class PluginViewKateFuzzyDocumentSwitcher: public Kate::PluginView, public Kate::XMLGUIClient {
  Q_OBJECT
  public:
    PluginViewKateFuzzyDocumentSwitcher(Kate::MainWindow *mainwindow);
    virtual ~PluginViewKateFuzzyDocumentSwitcher();
  private Q_SLOTS:
    void slotFuzzySwitch();

  private:
    KTextEditor::Document* m_prevDoc;
};


class PluginViewKateFuzzyDocumentSwitcherDialog: public KDialog {
    Q_OBJECT
    public:
        PluginViewKateFuzzyDocumentSwitcherDialog(QWidget *parent, KTextEditor::Document* docToSelect = 0);
        virtual ~PluginViewKateFuzzyDocumentSwitcherDialog();
        static KTextEditor::Document *document(QWidget *parent, KTextEditor::Document* docToSelect = 0);
    protected:
        bool eventFilter(QObject *obj, QEvent *event);
    private Q_SLOTS:
        void reselectFirst();
    private:
        SortFuzzyFilterProxyModel *m_model;
        QTreeView *m_listView;
        KLineEdit *m_inputLine;
};


#endif // _KateFuzzyDocumentSwitcher_h_

