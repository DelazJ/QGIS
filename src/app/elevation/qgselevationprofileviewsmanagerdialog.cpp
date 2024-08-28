/***************************************************************************
  qgselevationprofileviewsmanager.cpp
  --------------------------------------
  Date                 : Septembre 2024
  Copyright            : (C) 2024 by Harrissou Sant-anna
  Email                : delazj at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgselevationprofileviewsmanager.h"

#include "qgisapp.h"
#include "qgsnewnamedialog.h"
#include "qgsmapviewsmanager.h"
//#include "qgs3dmapcanvaswidget.h"
#include "qgsdockablewidgethelper.h"

#include <QMessageBox>

QgsElevationProfileViewsManagerDialog::QgsElevationProfileViewsManagerDialog( QWidget *parent, Qt::WindowFlags f )
  : QDialog( parent, f )
{
  setupUi( this );

  mListModel = new QStringListModel( this );
  mElevationProfilesListView->setModel( mListModel );

  mElevationProfilesListView->setEditTriggers( QAbstractItemView::NoEditTriggers );
  mElevationProfilesListView->setSelectionMode( QAbstractItemView::SingleSelection );

  connect( mButtonBox, &QDialogButtonBox::rejected, this, &QWidget::close );
  connect( mButtonBox, &QDialogButtonBox::helpRequested, this,  &QgsElevationProfileViewsManagerDialog::showHelp );

  connect( mShowButton, &QToolButton::clicked, this, &QgsElevationProfileViewsManagerDialog::showClicked );
  connect( mDuplicateButton, &QToolButton::clicked, this, &QgsElevationProfileViewsManagerDialog::duplicateClicked );
  connect( mRemoveButton, &QToolButton::clicked, this, &QgsElevationProfileViewsManagerDialog::removeClicked );
  connect( mRenameButton, &QToolButton::clicked, this, &QgsElevationProfileViewsManagerDialog::renameClicked );
  mShowButton->setEnabled( false );

  connect( mElevationProfilesListView->selectionModel(), &QItemSelectionModel::currentChanged, this, &QgsElevationProfileViewsManagerDialog::currentChanged );

  connect( QgsProject::instance()->profileViewsManager(), &QgsMapViewsManager::elevationprofilesListChanged, this, &QgsElevationProfileViewsManagerDialog::onElevationProfilesListChanged );
  mElevationProfilesListView->selectionModel()->setCurrentIndex( mElevationProfilesListView->model()->index( 0, 0 ), QItemSelectionModel::Select );
  currentChanged( mElevationProfilesListView->selectionModel()->currentIndex(), mElevationProfilesListView->selectionModel()->currentIndex() );
}

void QgsElevationProfileViewsManagerDialog::onElevationProfilesListChanged()
{
  reload();
}

void QgsElevationProfileViewsManagerDialog::showClicked()
{
  if ( mElevationProfilesListView->selectionModel()->selectedRows().isEmpty() )
    return;

  QString viewName = mElevationProfilesListView->selectionModel()->selectedRows().at( 0 ).data( Qt::DisplayRole ).toString();

  QgisApp::instance()->openElevationProfileView( viewName );

  mElevationProfilesListView->selectionModel()->setCurrentIndex( mElevationProfilesListView->selectionModel()->currentIndex(), QItemSelectionModel::Select );
  currentChanged( mElevationProfilesListView->selectionModel()->currentIndex(), mElevationProfilesListView->selectionModel()->currentIndex() );
}

void QgsElevationProfileViewsManagerDialog::hideClicked()
{
  if ( mElevationProfilesListView->selectionModel()->selectedRows().isEmpty() )
    return;

  QString viewName = mElevationProfilesListView->selectionModel()->selectedRows().at( 0 ).data( Qt::DisplayRole ).toString();

  QgisApp::instance()->closeElevationProfileView( viewName );

  mElevationProfilesListView->selectionModel()->setCurrentIndex( mElevationProfilesListView->selectionModel()->currentIndex(), QItemSelectionModel::Select );
  currentChanged( mElevationProfilesListView->selectionModel()->currentIndex(), mElevationProfilesListView->selectionModel()->currentIndex() );
}

void QgsElevationProfileViewsManagerDialog::duplicateClicked()
{
  if ( mElevationProfilesListView->selectionModel()->selectedRows().isEmpty() )
    return;

  QString existingViewName = mElevationProfilesListView->selectionModel()->selectedRows().at( 0 ).data( Qt::DisplayRole ).toString();
  QString newViewName = askUserForATitle( existingViewName, tr( "Duplicate" ), false );

  if ( newViewName.isEmpty() )
    return;

  QgisApp::instance()->duplicateElevationProfileMapView( existingViewName, newViewName );

  QgsProject::instance()->setDirty();
}

void QgsElevationProfileViewsManagerDialog::removeClicked()
{
  if ( mElevationProfilesListView->selectionModel()->selectedRows().isEmpty() )
    return;

  QString warningTitle = tr( "Remove Elevation Profile View" );
  QString warningMessage = tr( "Do you really want to remove selected elevation profile view?" );

  if ( QMessageBox::warning( this, warningTitle, warningMessage, QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
    return;

  QString viewName = mElevationProfilesListView->selectionModel()->selectedRows().at( 0 ).data( Qt::DisplayRole ).toString();

  QgsProject::instance()->profileViewsManager()->removeElevationProfileView( viewName );
  QgisApp::instance()->closeElevationProfileMapView( viewName );
  QgsProject::instance()->setDirty();
}

void QgsElevationProfileViewsManagerDialog::renameClicked()
{
  if ( mElevationProfilesListView->selectionModel()->selectedRows().isEmpty() )
    return;

  QString oldTitle = mElevationProfilesListView->selectionModel()->selectedRows().at( 0 ).data( Qt::DisplayRole ).toString();
  QString newTitle = askUserForATitle( oldTitle, tr( "Rename" ), true );

  if ( newTitle.isEmpty() )
    return;

  QgsProject::instance()->profileViewsManager()->renameElevationProfileView( oldTitle, newTitle );

  if ( QgsElevationProfileMapCanvasWidget *widget = QgisApp::instance()->getElevationProfileMapView( oldTitle ) )
  {
    widget->setCanvasName( newTitle );
  }

  QgsProject::instance()->setDirty();
}

void QgsElevationProfileViewsManagerDialog::currentChanged( const QModelIndex &current, const QModelIndex &previous )
{
  Q_UNUSED( previous );

  mRenameButton->setEnabled( current.isValid() );
  mRemoveButton->setEnabled( current.isValid() );
  mDuplicateButton->setEnabled( current.isValid() );
  if ( !current.isValid() )
  {
    mShowButton->setEnabled( false );
    return;
  }

  QString viewName = current.data( Qt::DisplayRole ).toString();
  bool isOpen = QgsProject::instance()->profileViewsManager()->isElevationProfileViewOpen( viewName );
  mShowButton->setEnabled( !isOpen );
}

void QgsElevationProfileViewsManagerDialog::reload()
{
  QStringList names = QgsProject::instance()->profileViewsManager()->getElevationProfileViewsNames();
  mListModel->setStringList( names );
}

QString QgsElevationProfileViewsManagerDialog::askUserForATitle( QString oldTitle, QString action, bool allowExistingTitle )
{
  QString newTitle = oldTitle;
  QStringList notAllowedTitles = mListModel->stringList();
  if ( allowExistingTitle )
    notAllowedTitles.removeOne( oldTitle );
  QgsNewNameDialog dlg( tr( "Elevation Profile view" ), newTitle, QStringList(), notAllowedTitles, Qt::CaseSensitive, this );
  dlg.setWindowTitle( tr( "%1 Elevation Profile View" ).arg( action ) );
  dlg.setHintString( tr( "Enter a unique elevation profile view title" ) );
  dlg.setOverwriteEnabled( false );
  dlg.setAllowEmptyName( false );
  dlg.setConflictingNameWarning( tr( "Title already exists!" ) );

  if ( dlg.exec() != QDialog::Accepted )
    return QString();
  newTitle = dlg.name();
  return newTitle;
}


void QgsElevationProfileViewsManagerDialog::showHelp()
{

}