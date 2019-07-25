#include"TechInfoDialog.h"
#include"TechInfoDialogData.h"
#include"ui_TechInfoDialog.h"
#include"ElapsedTimer.h"

#include<QTimer>

TechInfoDialog::TechInfoDialog(QWidget *parent):QDialog(parent),ui(new Ui::TechInfoDialog){
    ui->setupUi(this);

    for(auto attr:techInfoAttributes){
        findChild<QLineEdit*>(attr.lineEditName)->setText(attr.text);
    }

    connect(findChild<QTabWidget*>("tabWidget"),&QTabWidget::currentChanged,this,&TechInfoDialog::tabWidgetCurrentChanged);
    connect(findChild<QPushButton*>("refreshButton"),&QPushButton::clicked,this,&TechInfoDialog::refreshPerformanceTab);
    connect(findChild<QPushButton*>("aboutQtButton"),&QPushButton::clicked,QApplication::aboutQt);
}

TechInfoDialog::~TechInfoDialog(){
    delete ui;
}

void TechInfoDialog::tabWidgetCurrentChanged(int index){
    if(index==-1){
        return;
    }

    if(tabNames[index]=="performanceTab"){
        QTimer::singleShot(1,this,&TechInfoDialog::refreshPerformanceTab);
    }
}

void TechInfoDialog::refreshPerformanceTab(){
    QListWidget *performanceListWidget=findChild<QListWidget*>("performanceListWidget");

    performanceListWidget->clear();
    for(auto pair:ElapsedTimer::getAll()){
        performanceListWidget->addItem(QString("%1 = %2ms").arg(pair.first).arg(pair.second));
    }
}
