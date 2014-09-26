#uWebKit main qmake

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += UWKCommon UWKPlugin UWKProcess
    
UWKPlugin.depends = UWKCommon
UWKProcess.depends = UWKCommon
