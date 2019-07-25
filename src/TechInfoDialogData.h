#ifndef TECHINFODIALOGDATA_H
#define TECHINFODIALOGDATA_H

#include"version/Version.h"
#include<QList>
#include<QSysInfo>

typedef struct{
    QString lineEditName;
    QString text;
}TechInfoAttribute;

static const QList<TechInfoAttribute> techInfoAttributes{
    {"versionLineEdit",Version::versionString},
    {"gitCommitLineEdit",Version::lastCommitSha1},
    {"buildAbiLineEdit",QSysInfo::buildAbi()},
    {"buildCpuArchLineEdit",QSysInfo::buildCpuArchitecture()},
    {"currCpuArchLineEdit",QSysInfo::currentCpuArchitecture()},
    {"kernelTypeLineEdit",QSysInfo::kernelType()},
    {"kernelVersionLineEdit",QSysInfo::kernelVersion()},
    {"productNameLineEdit",QSysInfo::prettyProductName()},
    {"productTypeLineEdit",QSysInfo::productType()},
    {"productVersionLineEdit",QSysInfo::productVersion()}
};

static const QList<QString> tabNames{
    "systemTab",
    "performanceTab"
};

#endif // TECHINFODIALOGDATA_H
