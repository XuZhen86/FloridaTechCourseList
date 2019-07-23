#ifndef TECHNICALINFORMATIONDIALOGDATA_H
#define TECHNICALINFORMATIONDIALOGDATA_H

#include"version/Version.h"
#include<QList>
#include<QSysInfo>

typedef struct{
    QString lineEditName;
    QString text;
}TechnicalInformationAttribute;

static const QList<TechnicalInformationAttribute> technicalInformationAttributes{
    {"versionLineEdit",versionString},
    {"gitCommitLineEdit",lastCommitSha1},
    {"buildAbiLineEdit",QSysInfo::buildAbi()},
    {"buildCpuArchLineEdit",QSysInfo::buildCpuArchitecture()},
    {"currCpuArchLineEdit",QSysInfo::currentCpuArchitecture()},
    {"kernelTypeLineEdit",QSysInfo::kernelType()},
    {"kernelVersionLineEdit",QSysInfo::kernelVersion()},
    {"productNameLineEdit",QSysInfo::prettyProductName()},
    {"productTypeLineEdit",QSysInfo::productType()},
    {"productVersionLineEdit",QSysInfo::productVersion()}
};

#endif // TECHNICALINFORMATIONDIALOGDATA_H
