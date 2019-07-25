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
    findChild4Attributes();

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
        connect(attr.button,&QPushButton::clicked,this,&MainWindow::semesterButtonClicked);
    }

    for(auto attr:courseAttributes){
        if(attr.checkBoxName.isEmpty()){
            continue;
        }

        connect(attr.checkBox,&QCheckBox::stateChanged,this,&MainWindow::selectCheckBoxStateChanged);
    }

    for(auto attr:filterAttributes){
        connect(attr.checkBox,&QCheckBox::stateChanged,this,&MainWindow::filterCheckBoxStateChanged);
        connect(attr.comboBox,&QComboBox::currentTextChanged,this,&MainWindow::filterComboBoxCurrentTextChanged);
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

void MainWindow::findChild4Attributes(){
    for(auto &attr:courseAttributes){
        if(!attr.checkBoxName.isEmpty()){
            attr.checkBox=findChild<QCheckBox*>(attr.checkBoxName);
        }
    }

    for(auto &attr:semesterButtonAttributes){
        if(!attr.buttonName.isEmpty()){
            attr.button=findChild<QPushButton*>(attr.buttonName);
        }
    }

    for(auto &attr:filterAttributes){
        if(!attr.checkBoxName.isEmpty()){
            attr.checkBox=findChild<QCheckBox*>(attr.checkBoxName);
        }

        if(!attr.comboBoxName.isEmpty()){
            attr.comboBox=findChild<QComboBox*>(attr.comboBoxName);
        }
    }

    for(auto &attr:additionalInfoAttributes){
        if(!attr.plainTextEditName.isEmpty()){
            attr.plainTextEdit=findChild<QPlainTextEdit*>(attr.plainTextEditName);
        }
    }
}

void MainWindow::semesterButtonClicked(){
    QPushButton *button=dynamic_cast<QPushButton*>(sender());
    QString buttonName=button->objectName();
    auto attrIter=std::find_if(semesterButtonAttributes.begin(),semesterButtonAttributes.end(),
                               [&buttonName](SemesterButtonAttribute attr){return buttonName==attr.buttonName;});

    QList<QStringList> courseList;
    if(findChild<QLineEdit*>("secretCodeLineEdit")->text()=="StaticCourseList"){
        courseList=getCourseList(attrIter->staticCourseListFileName);
    }else{
        courseList=getCourseList(QUrl(attrIter->dynamicCourseListUrl));
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

    for(auto attr:filterAttributes){
        if(attr.comboBoxSetIndex==-1){
            continue;
        }

        QComboBox *comboBox=attr.comboBox;
        QStringList comboBoxStrList=comboBoxSet[attr.comboBoxSetIndex].toList();

        comboBoxStrList.append("");
        std::sort(comboBoxStrList.begin(),comboBoxStrList.end());

        comboBox->clear();
        comboBox->addItems(comboBoxStrList);
    }

    QTimer::singleShot(1,this,&MainWindow::selectCheckBoxStateChanged);
}

QList<QStringList> MainWindow::getCourseList(const QUrl &url){
    QNetworkAccessManager netAccMngr(this);
    QEventLoop eventLoop(this);
    QProgressDialog progressDialog("Requesting...","Cancel",0,0,this);

    QScopedPointer<QNetworkReply> networkReply(netAccMngr.get(QNetworkRequest(url)));

    connect(&netAccMngr,&QNetworkAccessManager::finished,
        [&eventLoop](){
            QTimer::singleShot(1,&eventLoop,&QEventLoop::quit);
        }
    );
    connect(&progressDialog,&QProgressDialog::canceled,
        [&eventLoop](){
            QTimer::singleShot(1,&eventLoop,&QEventLoop::quit);
        }
    );
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
        bool columnHidden=attr.columnAlwaysHidden||!attr.checkBox->isChecked();
        courseTable->setColumnHidden(attr.columnIndex,columnHidden);
    }
}

void MainWindow::filterCheckBoxStateChanged(){
    for(auto attr:filterAttributes){
        attr.comboBox->setDisabled(!attr.checkBox->isChecked());
    }

    QTimer::singleShot(1,this,&MainWindow::filterComboBoxCurrentTextChanged);
}

void MainWindow::filterComboBoxCurrentTextChanged(){
    QVector<bool> courseHidden(courseTable->rowCount(),false);

    for(int courseNum=0;courseNum<courseTable->rowCount();courseNum++){
        for(auto attr:filterAttributes){
            QCheckBox *checkBox=attr.checkBox;
            QComboBox *comboBox=attr.comboBox;
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
        QPlainTextEdit *plainTextEdit=attr.plainTextEdit;
        QString text=courseTable->item(currentRow,attr.columnIndex)->text();

        plainTextEdit->setPlainText(text);
    }
}
