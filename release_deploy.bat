copy ..\StoreSettings\shared\release\StoreSettings.dll ..\XeomaDetectorsRelease\
copy Report\LimeReport\lib64\release\limereport.dll ..\XeomaDetectorsRelease\
copy Report\LimeReport\lib64\release\QtZint.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5PrintSupport.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5Designer.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5DesignerComponents.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5Qml.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5Xml.dll ..\XeomaDetectorsRelease\
copy %QTDEPLOY%\Qt5Network.dll ..\XeomaDetectorsRelease\
MD ..\XeomaDetectorsRelease\printsupport
copy %QTDEPLOY%\..\plugins\printsupport\windowsprintersupport.dll ..\XeomaDetectorsRelease\printsupport\

