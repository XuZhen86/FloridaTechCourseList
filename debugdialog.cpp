#include"debugdialog.h"
#include"ui_debugdialog.h"
#include"version.h"

#include<algorithm>

struct DebugDialog::SysInfoAttr{
    QString key;
    QString value;
};

QList<DebugDialog::SysInfoAttr> DebugDialog::sysInfoAttrs{
    {"Version",        Version::versionString},
    {"Git Commit",     Version::lastCommitSha1},
    {"Build ABI",      QSysInfo::buildAbi()},
    {"Build CPU Arch.",QSysInfo::buildCpuArchitecture()},
    {"Curr. CPU Arch.",QSysInfo::currentCpuArchitecture()},
    {"Kern. Type",     QSysInfo::kernelType()},
    {"Kern. Ver.",     QSysInfo::kernelVersion()},
    {"Prod. Name",     QSysInfo::prettyProductName()},
    {"Prod. Type",     QSysInfo::productType()},
    {"Prod. Ver.",     QSysInfo::productVersion()}
};

void DebugDialog::sysInfoAttrsInit(){
    for(auto attr:sysInfoAttrs){
        sysPlainTextEdit->appendPlainText(
            QString("%1 = %2")
            .arg(attr.key,attr.value)
        );
    }

    sysPlainTextEdit->appendPlainText(QString());
}

DebugDialog::DebugDialog(QWidget *parent):QDialog(parent),ui(new Ui::DebugDialog){
    ui->setupUi(this);
    sysPlainTextEdit=findChild<QPlainTextEdit*>("sysPlainTextEdit");

    sysInfoAttrsInit();
}

DebugDialog::~DebugDialog(){
    delete ui;
}
