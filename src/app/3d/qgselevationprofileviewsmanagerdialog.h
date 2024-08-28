/***************************************************************************
  qgselevationprofileviewsmanager.h
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

#ifndef QGSELEVATIONPROFILEVIEWSMANAGERDIALOG_H
#define QGSELEVATIONPROFILEVIEWSMANAGERDIALOG_H

#include "ui_qgselevationprofileviewsmanagerdialog.h"

#include <QDialog>
#include <QStringListModel>
#include <QDomElement>

//class Qgs3DMapCanvasDockWidget; ???
//class QgsElevationProfileWidget; 

class QgsElevationProfileViewsManagerDialog : public QDialog, private Ui::QgsElevationProfileViewsManagerDialog
{
    Q_OBJECT

  public:
    explicit QgsElevationProfileViewsManagerDialog( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );

    void reload();

  private slots:
    void showClicked();
    void hideClicked();
    void duplicateClicked();
    void removeClicked();
    void renameClicked();
    void showHelp();

    void currentChanged( const QModelIndex &current, const QModelIndex &previous );

    void onElevationProfilesListChanged();
  private:
    QStringListModel *mListModel = nullptr;

    QString askUserForATitle( QString oldTitle, QString action, bool allowExistingTitle );
};

#endif // QGSELEVATIONPROFILEVIEWSMANAGERDIALOG_H
