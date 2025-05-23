# The following has been generated automatically from src/core/mesh/qgsmesh3daveraging.h
QgsMesh3DAveragingMethod.MultiLevelsAveragingMethod = QgsMesh3DAveragingMethod.Method.MultiLevelsAveragingMethod
QgsMesh3DAveragingMethod.SigmaAveragingMethod = QgsMesh3DAveragingMethod.Method.SigmaAveragingMethod
QgsMesh3DAveragingMethod.RelativeHeightAveragingMethod = QgsMesh3DAveragingMethod.Method.RelativeHeightAveragingMethod
QgsMesh3DAveragingMethod.ElevationAveragingMethod = QgsMesh3DAveragingMethod.Method.ElevationAveragingMethod
try:
    QgsMesh3DAveragingMethod.createFromXml = staticmethod(QgsMesh3DAveragingMethod.createFromXml)
    QgsMesh3DAveragingMethod.__abstract_methods__ = ['writeXml', 'readXml', 'equals', 'clone']
    QgsMesh3DAveragingMethod.__group__ = ['mesh']
except (NameError, AttributeError):
    pass
try:
    QgsMeshMultiLevelsAveragingMethod.__overridden_methods__ = ['writeXml', 'readXml', 'equals', 'clone']
    QgsMeshMultiLevelsAveragingMethod.__group__ = ['mesh']
except (NameError, AttributeError):
    pass
try:
    QgsMeshSigmaAveragingMethod.__overridden_methods__ = ['writeXml', 'readXml', 'equals', 'clone']
    QgsMeshSigmaAveragingMethod.__group__ = ['mesh']
except (NameError, AttributeError):
    pass
try:
    QgsMeshRelativeHeightAveragingMethod.__overridden_methods__ = ['writeXml', 'readXml', 'equals', 'clone']
    QgsMeshRelativeHeightAveragingMethod.__group__ = ['mesh']
except (NameError, AttributeError):
    pass
try:
    QgsMeshElevationAveragingMethod.__overridden_methods__ = ['writeXml', 'readXml', 'equals', 'clone']
    QgsMeshElevationAveragingMethod.__group__ = ['mesh']
except (NameError, AttributeError):
    pass
