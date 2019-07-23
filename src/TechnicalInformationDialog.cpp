#include"TechnicalInformationDialog.h"
#include"TechnicalInformationDialogData.h"
#include"ui_TechnicalInformationDialog.h"

#include<QMessageBox>

TechnicalInformationDialog::TechnicalInformationDialog(QWidget *parent):QDialog(parent),ui(new Ui::TechnicalInformationDialog){
    ui->setupUi(this);

    for(auto attr:technicalInformationAttributes){
        findChild<QLineEdit*>(attr.lineEditName)->setText(attr.text);
    }

    connect(findChild<QPushButton*>("aboutQtButton"),&QPushButton::clicked,
            [this](){
                QMessageBox::aboutQt(this,"About Qt");
            }
    );
}

TechnicalInformationDialog::~TechnicalInformationDialog(){
    delete ui;
}
