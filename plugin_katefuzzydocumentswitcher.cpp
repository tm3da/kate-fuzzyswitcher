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

#include "plugin_katefuzzydocumentswitcher.h"
#include "plugin_katefuzzydocumentswitcher.moc"
#include "sortfuzzyfilterproxymodel.h"

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>

#include <klineedit.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <qtreeview.h>
#include <qwidget.h>
#include <qboxlayout.h>
#include <qstandarditemmodel.h>
#include <qpointer.h>
#include <qevent.h>
#include <qlabel.h>
#include <qcoreapplication.h>
#include <QDesktopWidget>


Q_DECLARE_METATYPE(QPointer<KTextEditor::Document>)

const int DocumentRole=Qt::UserRole+1;
const int SortFilterRole=Qt::UserRole+2;

K_PLUGIN_FACTORY(KateFuzzyDocumentSwitcherFactory, registerPlugin<PluginKateFuzzyDocumentSwitcher>();)
K_EXPORT_PLUGIN(KateFuzzyDocumentSwitcherFactory(KAboutData("katefuzzydocumentswitcher","katefuzzydocumentswitcherplugin",ki18n("Fuzzy Document Switcher"), "0.1", ki18n("Fuzzyly switch between documents"), KAboutData::License_LGPL_V2)) )

//BEGIN: Plugin

PluginKateFuzzyDocumentSwitcher::PluginKateFuzzyDocumentSwitcher( QObject* parent , const QList<QVariant>&):
    Kate::Plugin ( (Kate::Application *)parent, "kate-fuzzy-document-switcher-plugin" ) {
}

PluginKateFuzzyDocumentSwitcher::~PluginKateFuzzyDocumentSwitcher() {
}

Kate::PluginView *PluginKateFuzzyDocumentSwitcher::createView (Kate::MainWindow *mainWindow) {
    return new PluginViewKateFuzzyDocumentSwitcher(mainWindow);
}

//END: Plugin

//BEGIN: View
PluginViewKateFuzzyDocumentSwitcher::PluginViewKateFuzzyDocumentSwitcher(Kate::MainWindow *mainwindow):
    Kate::PluginView(mainwindow),
    Kate::XMLGUIClient(KateFuzzyDocumentSwitcherFactory::componentData()),
    m_prevDoc(0) {

    KAction *a = actionCollection()->addAction("documents_fuzzyswitch");
    a->setText(i18n("Fuzzyswitch"));
    a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Tab) );
    connect( a, SIGNAL( triggered(bool) ), this, SLOT( slotFuzzySwitch() ) );

    mainwindow->guiFactory()->addClient (this);

}

PluginViewKateFuzzyDocumentSwitcher::~PluginViewKateFuzzyDocumentSwitcher() {
      mainWindow()->guiFactory()->removeClient (this);
}

void PluginViewKateFuzzyDocumentSwitcher::slotFuzzySwitch() {
    KTextEditor::Document *doc=PluginViewKateFuzzyDocumentSwitcherDialog::document(mainWindow()->window(), m_prevDoc);
    if (doc) {
        // before switching save current document as alternate one so it
        // would be preselected on a next switch for convenience
        KTextEditor::Document* currentDocument = mainWindow()->activeView() ? mainWindow()->activeView()->document() : 0;
        if(currentDocument != m_prevDoc)
            m_prevDoc = currentDocument;

        mainWindow()->activateView(doc);
    }
}
//END: View

//BEGIN: Dialog
KTextEditor::Document *PluginViewKateFuzzyDocumentSwitcherDialog::document(QWidget *parent, KTextEditor::Document* docToSelect) {
    PluginViewKateFuzzyDocumentSwitcherDialog dlg(parent, docToSelect);
    if (QDialog::Accepted==dlg.exec()) {
        // document ptr is held in the (row,0) , while item at (row, 1) might be selected,
        // we need to obtain an index in (row,0)
        QModelIndex selectedIdx = dlg.m_listView->currentIndex();
        QModelIndex idx(dlg.m_listView->model()->index(selectedIdx.row(), 0));
        if (idx.isValid()) {
            QVariant _doc=idx.data(DocumentRole);
            QPointer<KTextEditor::Document> doc=_doc.value<QPointer<KTextEditor::Document> >();
            return (KTextEditor::Document*)doc;
        }
    }
    return 0;
}

PluginViewKateFuzzyDocumentSwitcherDialog::PluginViewKateFuzzyDocumentSwitcherDialog(QWidget *parent, KTextEditor::Document* docToSelect):
    KDialog(parent) {
    setModal(true);

    setButtons( KDialog::Ok | KDialog::Cancel);
    setButtonGuiItem( KDialog::User1 , KGuiItem("Switch to") );
    showButtonSeparator(true);
    setCaption(i18n("Document Fuzzy Switch"));

    QWidget *mainwidget=new QWidget(this);

    QVBoxLayout *layout=new QVBoxLayout(mainwidget);
    layout->setSpacing(spacingHint());


    m_inputLine=new KLineEdit(mainwidget);


    QLabel *label=new QLabel(i18n("&Filter:"),this);
    label->setBuddy(m_inputLine);

    QHBoxLayout *subLayout=new QHBoxLayout();
    subLayout->setSpacing(spacingHint());

    subLayout->addWidget(label);
    subLayout->addWidget(m_inputLine,1);

    layout->addLayout(subLayout,0);

    m_listView=new QTreeView(mainwidget);
    layout->addWidget(m_listView,1);
    m_listView->setTextElideMode(Qt::ElideLeft);

    m_listView->setAlternatingRowColors(true);

    setMainWidget(mainwidget);
    m_inputLine->setFocus(Qt::OtherFocusReason);

    QStandardItemModel *base_model=new QStandardItemModel(0,2,this);
    QList<KTextEditor::Document*> docs=Kate::application()->documentManager()->documents();
    int linecount=0;
    QModelIndex idxToSelect;

    foreach(KTextEditor::Document *doc,docs) {
        QStandardItem *itemName=new QStandardItem(doc->documentName());

        itemName->setData(qVariantFromValue(QPointer<KTextEditor::Document>(doc)),DocumentRole);
        itemName->setData(QString("%1: %2").arg(doc->documentName()).arg(doc->url().prettyUrl()),SortFilterRole);

        itemName->setEditable(false);
        QFont font=itemName->font();
        font.setBold(true);
        itemName->setFont(font);

        QStandardItem *itemUrl = new QStandardItem(doc->url().prettyUrl());
        itemUrl->setEditable(false);
        base_model->setItem(linecount,0,itemName);
        base_model->setItem(linecount,1,itemUrl);
        linecount++;

        if(doc == docToSelect)
            idxToSelect = itemName->index();
    }

    m_model=new SortFuzzyFilterProxyModel(this);
    m_model->setFilterRole(SortFilterRole);
    m_model->setSortRole(SortFilterRole);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_model->setSortCaseSensitivity(Qt::CaseInsensitive);


    connect(m_inputLine,SIGNAL(textChanged(const QString&)),m_model,SLOT(setFuzzyFilterString(const QString&)));
    connect(m_inputLine,SIGNAL(textChanged(const QString&)),this,SLOT(reselectFirst()));
    connect(m_model,SIGNAL(rowsInserted(const QModelIndex &,int,int)),this,SLOT(reselectFirst()));
    connect(m_model,SIGNAL(rowsRemoved(const QModelIndex &,int,int)),this,SLOT(reselectFirst()));

    connect(m_listView,SIGNAL(activated(const QModelIndex &)),this,SLOT(accept()));

    m_listView->setModel(m_model);
    m_model->setSourceModel(base_model);

    if(idxToSelect.isValid())
        m_listView->setCurrentIndex(m_model->mapFromSource(idxToSelect));
    else
        reselectFirst();

    m_inputLine->installEventFilter(this);
    m_listView->installEventFilter(this);
    m_listView->setHeaderHidden(true);
    m_listView->setRootIsDecorated(false);
    QDesktopWidget *desktop=new QDesktopWidget();
    setMinimumWidth(desktop->screenGeometry(parent).width()/2);
    delete desktop;
    m_listView->resizeColumnToContents(0);
}

bool PluginViewKateFuzzyDocumentSwitcherDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent *keyEvent=static_cast<QKeyEvent*>(event);
        if (obj==m_inputLine) {
            if ( (keyEvent->key()==Qt::Key_Up) || (keyEvent->key()==Qt::Key_Down) ) {
                QCoreApplication::sendEvent(m_listView,event);
                return true;
            }
        } else {
            if ( (keyEvent->key()!=Qt::Key_Up) && (keyEvent->key()!=Qt::Key_Down) && (keyEvent->key()!=Qt::Key_Tab) && (keyEvent->key()!=Qt::Key_Backtab)) {
                QCoreApplication::sendEvent(m_inputLine,event);
                return true;
            }
        }
    }
    return KDialog::eventFilter(obj,event);
}

void PluginViewKateFuzzyDocumentSwitcherDialog::reselectFirst() {
    QModelIndex index=m_model->index(0,0);
    m_listView->setCurrentIndex(index);
    enableButton(KDialog::Ok,index.isValid());
}

PluginViewKateFuzzyDocumentSwitcherDialog::~PluginViewKateFuzzyDocumentSwitcherDialog() {
}


//END: Dialog

