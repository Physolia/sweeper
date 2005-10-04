/**
  * Copyright (c) 2003-2005 Ralf Hoelzer <ralf@well.com>
  *
  *  This program is free software; you can redistribute it and/or modify
  *  it under the terms of the GNU Lesser General Public License as published
  *  by the Free Software Foundation; either version 2.1 of the License, or
  *  (at your option) any later version.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU Lesser General Public License for more details.
  *
  *  You should have received a copy of the GNU Lesser General Public License
  *  along with this program; if not, write to the Free Software
  *  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.
  */

#ifndef PRIVACY_H
#define PRIVACY_H

#include <kdialog.h>
#include <klistview.h>

#include <QLinkedList>

#include "kprivacydialog.h"
#include "kprivacymanager.h"
#include "kprivacysettings.h"

class Privacy: public KDialog
{
    Q_OBJECT

signals:
    // TODO remove
    void changed(bool);
        
public:
    Privacy( QWidget *parent=0 );
    ~Privacy();

    virtual void load();
    virtual void save();
    virtual void defaults();

public slots:
    void cleanup();
    void selectAll();
    void selectNone();

private:
    KPrivacyDialog  *cleaningDialog;
    KPrivacySettings  *p3pSettings;
    KPrivacyManager *m_privacymanager;

    QLinkedList<Q3CheckListItem*> checklist;

    KListViewItem *generalCLI;
    KListViewItem *webbrowsingCLI;

    Q3CheckListItem *clearThumbnails;	
    Q3CheckListItem *clearRunCommandHistory;
    Q3CheckListItem *clearAllCookies;
    Q3CheckListItem *clearSavedClipboardContents;
    Q3CheckListItem *clearWebHistory;
    Q3CheckListItem *clearWebCache;
    Q3CheckListItem *clearFormCompletion;
    Q3CheckListItem *clearRecentDocuments;
    Q3CheckListItem *clearQuickStartMenu;
    Q3CheckListItem *clearFavIcons;
    //QCheckListItem *clearFileDialogHistory;


};

#endif
