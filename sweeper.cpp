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
  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  */

#include "sweeper.h"

#include "privacyaction.h"
#include "privacyfunctions.h"
#include "sweeperadaptor.h"

#include <KActionCollection>
#include <KConfig>
#include <KMessageBox>
#include <KSharedConfig>
#include <KStandardAction>
#include <QDBusConnection>

Sweeper::Sweeper(bool automatic)
   : KXmlGuiWindow(nullptr)
   , m_privacyConfGroup(KSharedConfig::openConfig(QStringLiteral("kprivacyrc"), KConfig::NoGlobals), "Cleaning")
   , m_automatic(automatic)
{
   QWidget *mainWidget = new QWidget(this);
   ui.setupUi(mainWidget);
   setCentralWidget(mainWidget);

   QTreeWidget *sw = ui.privacyListView;

   KStandardAction::quit(this, &Sweeper::close, actionCollection());

   createGUI(QStringLiteral("sweeperui.rc"));

   setAutoSaveSettings();

   generalCLI     = new QTreeWidgetItem(QStringList() << i18nc("General system content", "General"));
   sw->addTopLevelItem(generalCLI);
   webbrowsingCLI = new QTreeWidgetItem(QStringList() << i18nc("Web browsing content", "Web Browsing"));
   sw->addTopLevelItem(webbrowsingCLI);

   generalCLI->setExpanded(true);
   webbrowsingCLI->setExpanded(true);

   this->InitActions();


   connect(ui.cleanupButton, &QPushButton::clicked, this, &Sweeper::cleanup);
   connect(ui.selectAllButton, &QPushButton::clicked, this, &Sweeper::selectAll);
   connect(ui.selectNoneButton, &QPushButton::clicked, this, &Sweeper::selectNone);

   new KsweeperAdaptor(this);
   QDBusConnection::sessionBus().registerObject(QStringLiteral("/ksweeper"), this);

   load();

   if(automatic) {
      cleanup();
      close();
   }
}


Sweeper::~Sweeper()
{
   save();
}


void Sweeper::load()
{
   QVector<PrivacyAction*>::iterator itr;

   for (itr = checklist.begin(); itr != checklist.end(); ++itr) {
      (*itr)->setCheckState(0, m_privacyConfGroup.readEntry((*itr)->configKey(), true) ? Qt::Checked : Qt::Unchecked);
   }
}

void Sweeper::save()
{
   QVector<PrivacyAction*>::iterator itr;

   for (itr = checklist.begin(); itr != checklist.end(); ++itr) {
      m_privacyConfGroup.writeEntry((*itr)->configKey(), (*itr)->checkState(0) == Qt::Checked);
   }

   m_privacyConfGroup.sync();
}

void Sweeper::selectAll()
{
   QVector<PrivacyAction*>::iterator itr;

   for (itr = checklist.begin(); itr != checklist.end(); ++itr) {
      (*itr)->setCheckState(0, Qt::Checked);
   }

}

void Sweeper::selectNone()
{
   QVector<PrivacyAction*>::iterator itr;

   for (itr = checklist.begin(); itr != checklist.end(); ++itr) {
      (*itr)->setCheckState(0, Qt::Unchecked);
   }

}


void Sweeper::cleanup()
{
   if (!m_automatic) {
      if (KMessageBox::warningContinueCancel(this, i18n("You are deleting data that is potentially valuable to you. Are you sure?")) != KMessageBox::Continue) {
         return;
      }
   }

   ui.statusTextEdit->clear();
   ui.statusTextEdit->setText(i18n("Starting cleanup..."));

   QVector<PrivacyAction*>::iterator itr;

   for (itr = checklist.begin(); itr != checklist.end(); ++itr) {
      if((*itr)->checkState(0) == Qt::Checked) {
         QString statusText = i18n("Clearing %1...", (*itr)->text(0));
         ui.statusTextEdit->append(statusText);

         // actions return whether they were successful
         if(!(*itr)->action()) {
            QString errorText =  i18n("Clearing of %1 failed: %2", (*itr)->text(0), (*itr)->getErrMsg());
            ui.statusTextEdit->append(errorText);
         }
      }
   }

   ui.statusTextEdit->append(i18n("Clean up finished."));
}

void Sweeper::InitActions() {
    // store all entries in a list for easy access later on
   if (QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.kde.klipper"))) {
        checklist.append(new ClearSavedClipboardContentsAction(generalCLI));
   }
   checklist.append(new ClearRecentDocumentsAction(generalCLI));
   checklist.append(new ClearRunCommandHistoryAction(generalCLI));
   if (QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.kde.ActivityManager"))) {
        checklist.append( new ClearRecentApplicationAction( generalCLI ) );
   }
   checklist.append(new ClearThumbnailsAction(generalCLI));

   checklist.append(new ClearAllCookiesAction(webbrowsingCLI));
   checklist.append(new ClearFaviconsAction(webbrowsingCLI));
   checklist.append(new ClearWebHistoryAction(webbrowsingCLI));
   checklist.append(new ClearWebCacheAction(webbrowsingCLI));
   checklist.append(new ClearFormCompletionAction(webbrowsingCLI));
   checklist.append(new ClearAllCookiesPoliciesAction(webbrowsingCLI));

   ui.privacyListView->resizeColumnToContents(0);
}

// kate: tab-width 3; indent-mode cstyle; replace-tabs true;
