#include"MainWindow.h"
#include"MainWindowData.h"
#include"ui_MainWindow.h"

#include<QDebug>
#include<QFile>
#include<QJsonArray>
#include<QJsonDocument>
#include<QScopedPointer>
#include<QVector>
#include<QNetworkAccessManager>
#include<QEventLoop>
#include<QNetworkReply>
#include<QTimer>
#include<QProgressDialog>
#include<QMetaEnum>
#include<QMessageBox>

#include<algorithm>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    courseTable=findChild<QTableWidget*>("courseTable");

    std::sort(courseAttributes.begin(),courseAttributes.end());

    courseTable->setColumnCount(courseAttributes.size());
    QStringList horizontalLabels;
    for(auto attr:courseAttributes){
        horizontalLabels.append(attr.columnTitle);
        courseTable->setColumnHidden(attr.columnIndex,attr.columnAlwaysHidden);
        courseTable->setColumnWidth(attr.columnIndex,attr.columnWidth);
    }
    courseTable->setHorizontalHeaderLabels(horizontalLabels);
    connect(courseTable,&QTableWidget::currentCellChanged,this,&MainWindow::courseTableCurrentCellChanged);

    for(auto attr:semesterButtonAttributes){
        connect(findChild<QPushButton*>(attr.buttonName),&QPushButton::clicked,this,&MainWindow::semesterButtonClicked);
    }

    for(auto attr:courseAttributes){
        if(attr.checkBoxObjName.isEmpty()){
            continue;
        }
        connect(findChild<QCheckBox*>(attr.checkBoxObjName),&QCheckBox::stateChanged,this,&MainWindow::selectCheckBoxStateChanged);
    }

    for(auto attr:filterAttributes){
        connect(findChild<QCheckBox*>(attr.checkBoxName),&QCheckBox::stateChanged,this,&MainWindow::filterCheckBoxStateChanged);
        connect(findChild<QComboBox*>(attr.comboBoxName),&QComboBox::currentTextChanged,this,&MainWindow::filterComboBoxCurrentTextChanged);
    }

    std::sort(electiveAttributes.begin(),electiveAttributes.end());
    for(auto attr:electiveAttributes){
        findChild<QComboBox*>("descriptionComboBox")->addItem(attr.key);
    }

    techInfoDialog=new TechnicalInformationDialog(this);
    connect(findChild<QPushButton*>("technicalInformationButton"),&QPushButton::clicked,techInfoDialog,&TechnicalInformationDialog::show);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::semesterButtonClicked(){
    QPushButton *button=dynamic_cast<QPushButton*>(sender());
    QString buttonName=button->objectName();

    QList<QStringList> courseList;
    for(auto attr:semesterButtonAttributes){
        if(buttonName==attr.buttonName){
            if(findChild<QLineEdit*>("secretCodeLineEdit")->text()=="StaticCourseList"){
                courseList=getCourseList(attr.staticCourseListFileName);
            }else{
                courseList=getCourseList(QUrl(attr.dynamicCourseListUrl));
            }
        }
    }

    courseTable->clearSelection();
    courseTable->clearContents();
    courseTable->setRowCount(courseList.size());

    QVector<QSet<QString>> comboBoxSet(filterAttributes.size());
    for(int courseNum=0;courseNum<courseList.size();courseNum++){
        for(auto attr:courseAttributes){
            courseTable->setItem(
                        courseNum,
                        attr.columnIndex,
                        new QTableWidgetItem(courseList.at(courseNum).at(attr.columnIndex)));
            if(attr.filterComboBoxSetIndex!=-1){
                comboBoxSet[attr.filterComboBoxSetIndex].insert(courseList.at(courseNum).at(attr.columnIndex));
            }
        }
    }

    selectCheckBoxStateChanged();

    for(auto attr:filterAttributes){
        if(attr.comboBoxSetIndex==-1){
            continue;
        }

        QComboBox *comboBox=findChild<QComboBox*>(attr.comboBoxName);
        QStringList comboBoxStrList=comboBoxSet[attr.comboBoxSetIndex].toList();

        comboBoxStrList.append("");
        std::sort(comboBoxStrList.begin(),comboBoxStrList.end());

        comboBox->clear();
        comboBox->addItems(comboBoxStrList);
    }
}

QList<QStringList> MainWindow::getCourseList(const QUrl &url){
    QNetworkAccessManager netAccMngr(this);
    QEventLoop eventLoop(this);
    QProgressDialog progressDialog("Requesting...","Cancel",0,0,this);

    QScopedPointer<QNetworkReply> networkReply(netAccMngr.get(QNetworkRequest(url)));

    connect(&netAccMngr,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);
    connect(&progressDialog,&QProgressDialog::canceled,&eventLoop,&QEventLoop::quit);
    connect(networkReply.data(),&QNetworkReply::downloadProgress,
            [&progressDialog](long long bytesReceived,long long bytesTotal){
                progressDialog.setRange(0,static_cast<int>(bytesTotal));
                progressDialog.setValue(static_cast<int>(bytesReceived));
                progressDialog.setLabelText(QString("Received %L1 bytes...").arg(bytesReceived));
            }
    );

    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();
    eventLoop.exec();

    if(networkReply->error()!=QNetworkReply::NoError){
        QMetaObject metaObject=QNetworkReply::staticMetaObject;
        QMetaEnum metaEnum=metaObject.enumerator(metaObject.indexOfEnumerator("NetworkError"));
        QString errorString=QString("Error When Receiving Data:\n%1\n\nPlease check your Internet connection and try again").arg(metaEnum.valueToKey(networkReply->error()));
        QMessageBox::warning(this,"Error",errorString,QMessageBox::Ok);
        return QList<QStringList>();
    }

    return processJsonData(networkReply->readAll());
}

QList<QStringList> MainWindow::getCourseList(const QString &fileName){
    QFile file(fileName,this);

    for(int trial=0;!file.isOpen()&&trial<5;trial++){
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
            qWarning("File %s failed to open on trial %d",fileName.toLatin1().data(),trial);
        }
    }
    if(!file.isOpen()){
        qFatal("File %s failed to open",fileName.toLatin1().data());
    }

    return processJsonData(file.readAll());
}

QList<QStringList> MainWindow::processJsonData(const QByteArray &jsonData){
    QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonData);
    QList<QStringList> courseList;

    for(QJsonValue courseValue:jsonDoc["records"].toArray()){
        QStringList course;

        for(auto attr:courseAttributes){
            if(attr.columnTitle=="Location"){
                if(courseValue["building"]!="TBA"){
                    course.append(QString("%1 %2")
                                  .arg(courseValue["building"].toString())
                                  .arg(courseValue["room"].toString()));
                }else{
                    course.append("");
                }
            }else if(attr.columnTitle=="Begin\nTime"||attr.columnTitle=="End\nTime"){
                int timeNum=courseValue[attr.jsonName].toInt();
                if(timeNum){
                    QString timeString=QString("%1").arg(timeNum,4,10);
                    timeString.insert(timeString.length()/2,":");
                    course.append(timeString);
                }else{
                    course.append("");
                }
            }else{
                switch(attr.jsonType){
                    case QJsonValue::Double:
                        course.append(QString::number(courseValue[attr.jsonName].toInt()));
                        break;
                    case QJsonValue::String:
                        course.append(courseValue[attr.jsonName].toString().replace("&#039;","'"));
                        break;
                    default:
                        course.append("");
                }
            }
        }

        courseList.append(course);
    }

    qInfo("Read %d courses",courseList.size());
    return courseList;
}

void MainWindow::selectCheckBoxStateChanged(){
    for(auto attr:courseAttributes){
        courseTable->setColumnHidden(attr.columnIndex,
                                     attr.columnAlwaysHidden||!findChild<QCheckBox*>(attr.checkBoxObjName)->isChecked());
    }
}

void MainWindow::filterCheckBoxStateChanged(){
    QCheckBox *checkBox=dynamic_cast<QCheckBox*>(sender());
    QString checkBoxName=checkBox->objectName();

    bool foundFilterPair=false;
    for(auto attr:filterAttributes){
        if(checkBoxName==attr.checkBoxName){
            findChild<QComboBox*>(attr.comboBoxName)->setDisabled(!checkBox->isChecked());
            foundFilterPair=true;
        }
    }
    if(!foundFilterPair){
        qFatal("Unexpected CheckBox name %s",checkBoxName.toLatin1().data());
    }

    filterComboBoxCurrentTextChanged();
}

void MainWindow::filterComboBoxCurrentTextChanged(){
    QVector<bool> courseHidden(courseTable->rowCount(),false);

    for(int courseNum=0;courseNum<courseTable->rowCount();courseNum++){
        for(auto attr:filterAttributes){
            QCheckBox *checkBox=findChild<QCheckBox*>(attr.checkBoxName);
            QComboBox *comboBox=findChild<QComboBox*>(attr.comboBoxName);
            QString comboBoxText=comboBox->currentText();

            if(checkBox->isChecked()&&!comboBoxText.isEmpty()){
                QString courseText=courseTable->item(courseNum,attr.columnIndex)->text();

                if(attr.matchFullText){
                    if(courseText.compare(comboBoxText,attr.caseSensitivity)!=0){
                        courseHidden[courseNum]=true;
                        break;
                    }
                }else{
                    if(!courseText.contains(comboBoxText,attr.caseSensitivity)){
                        courseHidden[courseNum]=true;
                        break;
                    }
                }
            }
        }
    }

    for(int courseNum=0;courseNum<courseTable->rowCount();courseNum++){
        courseTable->setRowHidden(courseNum,courseHidden.at(courseNum));
    }
}

void MainWindow::courseTableCurrentCellChanged(int currentRow,int currentColumn,int previousRow,int previousColumn){
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);

    if(currentRow==-1||currentColumn==-1){
        return;
    }

    for(auto attr:additionalInfoAttributes){
        QPlainTextEdit *plainTextEdit=findChild<QPlainTextEdit*>(attr.plainTextEditName);
        QString text=courseTable->item(currentRow,attr.columnIndex)->text();

        plainTextEdit->setPlainText(text);
    }
}
