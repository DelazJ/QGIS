/***************************************************************************
                             qgsprojectutils.h
                             -------------------
    begin                : July 2021
    copyright            : (C) 2021 Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsprojectutils.h"
#include "qgsaction.h"
#include "qgsactionmanager.h"
#include "qgsapplication.h"
#include "qgsgrouplayer.h"
#include "qgslayertree.h"
#include "qgsmaplayerutils.h"
#include "qgsproject.h"
#include "qgsprojectstorage.h"
#include "qgsprojectstorageregistry.h"
#include "qgssettingsentryenumflag.h"
#include "qgssettingsentryimpl.h"
#include "qgssettingsregistrycore.h"

QList<QgsMapLayer *> QgsProjectUtils::layersMatchingPath( const QgsProject *project, const QString &path )
{
  QList<QgsMapLayer *> layersList;
  if ( !project )
    return layersList;

  const QMap<QString, QgsMapLayer *> mapLayers( project->mapLayers() );
  for ( QgsMapLayer *layer : mapLayers )
  {
    if ( QgsMapLayerUtils::layerSourceMatchesPath( layer, path ) )
    {
      layersList << layer;
    }
  }
  return layersList;
}

bool QgsProjectUtils::updateLayerPath( QgsProject *project, const QString &oldPath, const QString &newPath )
{
  if ( !project )
    return false;

  bool res = false;
  const QMap<QString, QgsMapLayer *> mapLayers( project->mapLayers() );
  for ( QgsMapLayer *layer : mapLayers )
  {
    if ( QgsMapLayerUtils::layerSourceMatchesPath( layer, oldPath ) )
    {
      res = QgsMapLayerUtils::updateLayerSourcePath( layer, newPath ) || res;
    }
  }
  return res;
}

bool QgsProjectUtils::layerIsContainedInGroupLayer( QgsProject *project, QgsMapLayer *layer )
{
  // two situations we need to catch here -- one is a group layer which isn't present in the layer
  // tree, the other is a group layer associated with the project's layer tree which contains
  // UNCHECKED child layers
  const QVector< QgsGroupLayer * > groupLayers = project->layers< QgsGroupLayer * >();
  for ( QgsGroupLayer *groupLayer : groupLayers )
  {
    if ( groupLayer->childLayers().contains( layer ) )
      return true;
  }

  std::function< bool( QgsLayerTreeGroup *group ) > traverseTree;
  traverseTree = [ &traverseTree, layer ]( QgsLayerTreeGroup * group ) -> bool
  {
    // is the group a layer group containing our target layer?
    if ( group->groupLayer() && group->findLayer( layer ) )
    {
      return true;
    }

    const QList< QgsLayerTreeNode * > children = group->children();
    for ( QgsLayerTreeNode *node : children )
    {
      if ( QgsLayerTreeGroup *childGroup = qobject_cast< QgsLayerTreeGroup * >( node ) )
      {
        if ( traverseTree( childGroup ) )
          return true;
      }
    }
    return false;
  };
  return traverseTree( project->layerTreeRoot() );
}

Qgis::ProjectTrustStatus QgsProjectUtils::checkUserTrust( QgsProject *project )
{
  const Qgis::EmbeddedScriptMode embeddedScriptMode = QgsSettingsRegistryCore::settingsCodeExecutionBehaviorUndeterminedProjects->value();
  switch ( embeddedScriptMode )
  {
    case Qgis::EmbeddedScriptMode::Always:
    {
      // A user having changed the behavior to always allow is considered as determined
      return Qgis::ProjectTrustStatus::Trusted;
    }

    case Qgis::EmbeddedScriptMode::Never:
    {
      // A user having changed the behavior to always deny is considered as determined
      return Qgis::ProjectTrustStatus::Untrusted;
    }

    case Qgis::EmbeddedScriptMode::Ask:
    case Qgis::EmbeddedScriptMode::NeverAsk:
    case Qgis::EmbeddedScriptMode::SessionOnly:
    case Qgis::EmbeddedScriptMode::NotForThisSession:
    {
      QString absoluteFilePath;
      QString absolutePath;
      QgsProjectStorage *storage = QgsApplication::projectStorageRegistry()->projectStorageFromUri( project->fileName() );
      if ( storage )
      {
        if ( !storage->filePath( project->fileName() ).isEmpty() )
        {
          QFileInfo fileInfo( storage->filePath( project->fileName() ) );
          absoluteFilePath = fileInfo.absoluteFilePath();
          absolutePath = fileInfo.absolutePath();
        }
        else
        {
          // Match non-file based URIs too
          absolutePath = project->fileName();
        }
      }
      else
      {
        QFileInfo fileInfo( project->fileName() );
        absoluteFilePath = fileInfo.absoluteFilePath();
        absolutePath = fileInfo.absolutePath();
      }

      const QStringList untrustedProjectsFolders = QgsSettingsRegistryCore::settingsCodeExecutionUntrustedProjectsFolders->value() + QgsApplication::temporarilyUntrustedProjectsFolders();
      for ( const QString &path : untrustedProjectsFolders )
      {
        if ( absoluteFilePath == path || absolutePath == path )
        {
          return Qgis::ProjectTrustStatus::Untrusted;
        }
      }

      const QStringList trustedProjectsFolders = QgsSettingsRegistryCore::settingsCodeExecutionTrustedProjectsFolders->value() + QgsApplication::temporarilyTrustedProjectsFolders();
      for ( const QString &path : trustedProjectsFolders )
      {
        if ( absoluteFilePath == path || absolutePath == path )
        {
          return Qgis::ProjectTrustStatus::Trusted;
        }
      }

      return Qgis::ProjectTrustStatus::Undetermined;
    }
  }

  return Qgis::ProjectTrustStatus::Undetermined;
}
