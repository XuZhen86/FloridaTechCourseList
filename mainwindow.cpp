#include"mainwindow.h"
#include"ui_mainwindow.h"
#include"version.h"

#include<QCompleter>
#include<QEventLoop>
#include<QFileDialog>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonParseError>
#include<QJsonValue>
#include<QMessageBox>
#include<QMetaEnum>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QProgressDialog>
#include<QScopedPointer>
#include<QTimer>
#include<QVector>
#include<algorithm>
#include<tuple>

struct MainWindow::CourseColumnAttr{
    QString          columnTitle;
    QString          jsonName;
    QJsonValue::Type jsonType;
    int              columnIndex;
    QString          selectCheckBoxName;
    QCheckBox        *selectCheckBox;
    bool             selectCheckBoxChecked;
    int              filterComboBoxSetIndex;
    bool             columnAlwaysHidden;
    int              columnWidth;

    bool operator<(const MainWindow::CourseColumnAttr &attr)const{
        return columnIndex<attr.columnIndex;
    }
};

QList<MainWindow::CourseColumnAttr> MainWindow::courseColumnAttrs{
    {"CRN",           "crn",          QJsonValue::Double,0, "crnSelectCheckBox",         nullptr,true, 0, false,60},
    {"Subject",       "subject",      QJsonValue::String,1, "subjectSelectCheckBox",     nullptr,true, 2, false,60},
    {"Course\nNumber","course_number",QJsonValue::String,2, "courseNumberSelectCheckBox",nullptr,true, 1, false,60},
    {"Title",         "title",        QJsonValue::String,3, "titleSelectCheckBox",       nullptr,true, 3, false,200},
    {"Instructor",    "instructor",   QJsonValue::String,4, "instructorSelectCheckBox",  nullptr,true, 4, false,150},
    {"Days",          "days",         QJsonValue::String,5, "daysSelectCheckBox",        nullptr,true, 5, false,60},
    {"Section",       "section",      QJsonValue::String,6, "sectionSelectCheckBox",     nullptr,true, -1,false,50},
    {"Begin\nTime",   "begin_time",   QJsonValue::Double,7, "beginTimeSelectCheckBox",   nullptr,true, -1,false,50},
    {"End\nTime",     "end_time",     QJsonValue::Double,8, "endTimeSelectCheckBox",     nullptr,true, -1,false,50},
    {"Actual\nEnroll","actual_enroll",QJsonValue::Double,9, "actualEnrollSelectCheckBox",nullptr,true, -1,false,50},
    {"Max\nEnroll",   "max_enroll",   QJsonValue::Double,10,"maxEnrollSelectCheckBox",   nullptr,true, -1,false,50},
    {"Description",   "description",  QJsonValue::String,11,"",                          nullptr,false,-1,true, 0},
    {"Credit\nHours", "credit_hours", QJsonValue::Double,12,"",                          nullptr,false,-1,true, 0},
    {"Location",      "",             QJsonValue::Null,  13,"",                          nullptr,false,-1,true, 0},
    {"Current\nTerm", "current_term", QJsonValue::String,14,"",                          nullptr,false,-1,true, 0},
    {"Term",          "term",         QJsonValue::String,15,"",                          nullptr,false,-1,true, 0}
};

void MainWindow::courseColumnAttrsInit(){
    std::sort(courseColumnAttrs.begin(),courseColumnAttrs.end());

    for(auto &attr:courseColumnAttrs){
        if(attr.selectCheckBoxName.isEmpty()){
            continue;
        }

        attr.selectCheckBox=findChild<QCheckBox*>(attr.selectCheckBoxName);
        attr.selectCheckBox->setChecked(attr.selectCheckBoxChecked);
    }
}

void MainWindow::courseColumnConnect(){
    for(const auto &attr:courseColumnAttrs){
        if(attr.selectCheckBox==nullptr){
            continue;
        }

        connect(
            attr.selectCheckBox,&QCheckBox::stateChanged,
            this,&MainWindow::selectCheckBoxStateChanged
        );
    }
}

struct MainWindow::SemesterButtonAttr{
    QString     buttonName;
    QPushButton *button;
    QString     staticCourseListFileName;
    QUrl        dynamicCourseListUrl;
    int         dynamicCourseListEstSize;
};

QList<MainWindow::SemesterButtonAttr> MainWindow::semesterButtonAttrs{
    {"springSemesterButton",nullptr,":/StaticCourseList/spring.json",QUrl("http://api.fit.edu/courses/v1/courses?term=spring"),1400000},
    {"summerSemesterButton",nullptr,":/StaticCourseList/summer.json",QUrl("http://api.fit.edu/courses/v1/courses?term=summer"),660000},
    {"fallSemesterButton",  nullptr,":/StaticCourseList/fall.json",  QUrl("http://api.fit.edu/courses/v1/courses?term=fall"),  1400000}
};

void MainWindow::semesterButtonAttrsInit(){
    for(auto &attr:semesterButtonAttrs){
        attr.button=findChild<QPushButton*>(attr.buttonName);
    }
}

void MainWindow::semesterButtonConnect(){
    for(const auto &attr:semesterButtonAttrs){
        connect(
            attr.button,&QPushButton::clicked,
            this,&MainWindow::semesterButtonClicked
        );
    }
}

struct MainWindow::FilterAttr{
    QString                    checkBoxName;
    QCheckBox                  *checkBox;
    QString                    comboBoxName;
    QComboBox                  *comboBox;
    int                        columnIndex;
    int                        comboBoxSetIndex;
    Qt::CaseSensitivity        caseSensitivity;
    bool                       matchFullText;
    QCompleter::CompletionMode completionMode;
};

QList<MainWindow::FilterAttr> MainWindow::filterAttrs{
    {"crnFilterCheckBox",         nullptr,"crnComboBox",         nullptr,0, 0, Qt::CaseInsensitive,true, QCompleter::PopupCompletion},
    {"courseNumberFilterCheckBox",nullptr,"courseNumberComboBox",nullptr,2, 1, Qt::CaseInsensitive,false,QCompleter::PopupCompletion},
    {"subjectFilterCheckBox",     nullptr,"subjectComboBox",     nullptr,1, 2, Qt::CaseInsensitive,true, QCompleter::PopupCompletion},
    {"titleFilterCheckBox",       nullptr,"titleComboBox",       nullptr,3, 3, Qt::CaseInsensitive,false,QCompleter::PopupCompletion},
    {"instructorFilterCheckBox",  nullptr,"instructorComboBox",  nullptr,4, 4, Qt::CaseInsensitive,true, QCompleter::PopupCompletion},
    {"daysFilterCheckBox",        nullptr,"daysComboBox",        nullptr,5, 5, Qt::CaseInsensitive,true, QCompleter::PopupCompletion},
    {"descriptionFilterCheckBox", nullptr,"descriptionComboBox", nullptr,11,-1,Qt::CaseSensitive,  false,QCompleter::PopupCompletion}
};

void MainWindow::filterAttrsInit(){
    for(auto &attr:filterAttrs){
        attr.checkBox=findChild<QCheckBox*>(attr.checkBoxName);
        attr.comboBox=findChild<QComboBox*>(attr.comboBoxName);

        if(attr.comboBox->completer()!=nullptr){
            attr.comboBox->completer()->setCompletionMode(attr.completionMode);
        }
    }
}

void MainWindow::filterAttrsValidate(){
    for(auto &fAttr:filterAttrs){
        assert(std::any_of(
            courseColumnAttrs.begin(),courseColumnAttrs.end(),
            [&fAttr](const CourseColumnAttr &ccAttr){
                return fAttr.columnIndex==ccAttr.columnIndex
                        &&fAttr.comboBoxSetIndex==ccAttr.filterComboBoxSetIndex;
            }
        ));
    }
}

void MainWindow::filterAttrsConnect(){
    for(const auto &attr:filterAttrs){
        connect(
            attr.checkBox,&QCheckBox::stateChanged,
            this,&MainWindow::filterCheckBoxStateChanged
        );

        connect(
            attr.comboBox,&QComboBox::currentTextChanged,
            this,&MainWindow::filterComboBoxCurrentTextChanged
        );
    }
}

void MainWindow::filterSetCourseList(const QList<QStringList> &courseList){
    QVector<QSet<QString>> comboBoxSet(filterAttrs.size());

    for(const auto &course:courseList){
        for(const auto &attr:filterAttrs){
            if(attr.comboBoxSetIndex==-1){
                continue;
            }

            comboBoxSet[attr.comboBoxSetIndex].insert(course.at(attr.columnIndex));
        }
    }

    for(const auto &attr:filterAttrs){
        if(attr.comboBoxSetIndex==-1){
            continue;
        }

        QStringList texts=comboBoxSet[attr.comboBoxSetIndex].toList();
        texts.append("");
        std::sort(texts.begin(),texts.end());

        attr.comboBox->clear();
        attr.comboBox->addItems(texts);
    }
}

struct MainWindow::AdditionalInfoAttr{
    QString        plainTextEditName;
    QPlainTextEdit *plainTextEdit;
    QString        columnTitle;
    int            columnIndex;
};

QList<MainWindow::AdditionalInfoAttr> MainWindow::additionalInfoAttrs{
    {"descriptionPlainTextEdit",nullptr,"Description",  11},
    {"creditHoursPlainTextEdit",nullptr,"Credit\nHours",12},
    {"locationPlainTextEdit",   nullptr,"Location",     13},
};

void MainWindow::additionalInfoAttrsInit(){
    for(auto &attr:additionalInfoAttrs){
        attr.plainTextEdit=findChild<QPlainTextEdit*>(attr.plainTextEditName);
    }
}

void MainWindow::additionalInfoAttrsValidate(){
    for(const auto &aiAttr:additionalInfoAttrs){
        assert(std::any_of(
            courseColumnAttrs.begin(),courseColumnAttrs.end(),
            [&aiAttr](const CourseColumnAttr &ccAttr){
                return aiAttr.columnTitle==ccAttr.columnTitle
                        &&aiAttr.columnIndex==ccAttr.columnIndex;
            }
        ));
    }
}

struct MainWindow::ElectiveAttr{
    QString name;
    QString key;

    bool operator<(const MainWindow::ElectiveAttr &a)const{
        return key<a.key;
    }
};

QList<MainWindow::ElectiveAttr> MainWindow::electiveAttrs{
    {"Humanities",               "(HU)"},
    {"Social Science",           "(SS)"},
    {"Liberal Arts",             "(LA)"},
    {"",                         "(CL)"},
    {"",                         "(Q)"},
    {"",                         "(CC)"},
    {"Humanities/Social Science","(HU/SS)"},
    {"",                         "(H)"}
};

void MainWindow::electiveAttrsInit(){
    electiveAttrs.append({"",""});
    std::sort(electiveAttrs.begin(),electiveAttrs.end());

    for(const auto &attr:electiveAttrs){
        findChild<QComboBox*>("descriptionComboBox")->addItem(attr.key);
    }
}

struct MainWindow::GroupBoxAttr{
    QString groupBoxName;
    QGroupBox *groupBox;
    bool enabled;
};

QList<MainWindow::GroupBoxAttr> MainWindow::groupBoxAttrs{
    {"courseListGroupBox",nullptr,false},
    {"additionalInfoGroupBox",nullptr,false},
    {"semesterGroupBox",nullptr,true},
    {"selectColumnsGroupBox",nullptr,false},
    {"filterGroupBox",nullptr,false},
    {"debugGroupBox",nullptr,true},
    {"advancedGroupBox",nullptr,false}
};

void MainWindow::groupBoxAttrsInit(){
    for(auto &attr:groupBoxAttrs){
        attr.groupBox=findChild<QGroupBox*>(attr.groupBoxName);
        attr.groupBox->setEnabled(attr.enabled);
    }
}

void MainWindow::groupBoxEnableAll(){
    for(auto attr:groupBoxAttrs){
        attr.groupBox->setEnabled(true);
    }
}

void MainWindow::attrsInit(){
    filterAttrsValidate();
    additionalInfoAttrsValidate();

    courseColumnAttrsInit();
    semesterButtonAttrsInit();
    filterAttrsInit();
    additionalInfoAttrsInit();
    electiveAttrsInit();
    groupBoxAttrsInit();

    courseColumnConnect();
    semesterButtonConnect();
    filterAttrsConnect();
}

void MainWindow::courseTableInit(){
    courseTable=findChild<QTableWidget*>("courseTable");
    courseTable->setColumnCount(courseColumnAttrs.size());

    for(const auto &attr:courseColumnAttrs){
        courseTable->setHorizontalHeaderItem(attr.columnIndex,new QTableWidgetItem(attr.columnTitle));
        courseTable->setColumnHidden(attr.columnIndex,attr.columnAlwaysHidden);
        courseTable->setColumnWidth(attr.columnIndex,attr.columnWidth);
    }

    connect(
        courseTable,&QTableWidget::currentCellChanged,
        this,&MainWindow::courseTableCurrentCellChanged
    );
}

void MainWindow::debugDialogInit(){
    debugDialog=new DebugDialog(this);
}

void MainWindow::debugDialogConnect(){
    connect(
        debugButton,&QPushButton::clicked,
        debugDialog,&DebugDialog::show
    );
}

void MainWindow::advancedSettingInit(){
    this->settings["useRegExpInFilter"]=Qt::Unchecked;
}

void MainWindow::advancedSettingConnect(){
    // Sync checkbox state with settings map
    connect(
        findChild<QCheckBox*>("useRegExpInFilterCheckBox"),&QCheckBox::stateChanged,
        [this](int state){
            this->settings["useRegExpInFilter"]=state;
            QMetaObject::invokeMethod(this,&MainWindow::filterCheckBoxStateChanged,Qt::QueuedConnection);
        }
    );
}

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    secretCodeLineEdit=findChild<QLineEdit*>("secretCodeLineEdit");
    debugButton=findChild<QPushButton*>("debugButton");

    currentVersionLabel=findChild<QLabel*>("currentVersionLabel");
    currentVersionLabel->setText(QString("Current Version: %1").arg(Version::versionString));

    attrsInit();
    courseTableInit();
    advancedSettingInit();
    advancedSettingConnect();
    debugDialogInit();
    debugDialogConnect();
}

MainWindow::~MainWindow(){
    delete ui;
}

QByteArray MainWindow::getAll(const QString &fileName){
    if(fileName.isEmpty()){
        return QByteArray();
    }

    QFile file(fileName,this);
    if(file.open(QFile::ReadOnly|QFile::Text)){
        return file.readAll();
    }

    return QByteArray();
}

QByteArray MainWindow::getAll(const QUrl &url,const int estSize){
    QEventLoop eventLoop(this);
    QProgressDialog progDialog("Requesting course list...","Cancel",0,0,this);
    QNetworkAccessManager netAccMngr(this);
    QScopedPointer<QNetworkReply> netReplyP(netAccMngr.get(QNetworkRequest(url)));
    QTimer timer(this);

    // A workaround that event loop will not always quit on first call
    timer.setInterval(200);
    connect(
        &timer,&QTimer::timeout,
        &eventLoop,&QEventLoop::quit
    );

    connect(
        netReplyP.get(),&QNetworkReply::finished, // Emitted when download finished AND when canceled
        [&timer](){
            timer.start();
        }
    );

    connect(
        netReplyP.get(),&QNetworkReply::downloadProgress,
        [&progDialog,estSize](long long bytesReceived,long long bytesTotal){
            if(progDialog.isHidden()){ // Not allowing dialog to briefly reappear after cancel
                return;
            }

            if(bytesTotal==-1){
                progDialog.setRange(0,estSize);
            }else{
                progDialog.setRange(0,static_cast<int>(bytesTotal));
            }

            progDialog.setValue(static_cast<int>(bytesReceived));
            progDialog.setLabelText(
                QString("Received %L1 bytes...")
                .arg(bytesReceived)
            );
        }
    );

    progDialog.setWindowModality(Qt::WindowModal);
    progDialog.setAutoClose(false);
    progDialog.setAutoReset(false);
    progDialog.show();
    connect(
        &progDialog,
        &QProgressDialog::canceled,
        [&netReplyP](){
            netReplyP->abort();
        }
    );

    eventLoop.exec();

    // Pretty print error message
    if(netReplyP->error()!=QNetworkReply::NoError
       &&netReplyP->error()!=QNetworkReply::OperationCanceledError){
        QMetaObject metaObject=QNetworkReply::staticMetaObject;
        QMetaEnum metaEnum=metaObject.enumerator(metaObject.indexOfEnumerator("NetworkError"));
        QString errorString=
            QString("Error When Receiving Data:\n%1\n\nPlease check your Internet connection and try again")
            .arg(metaEnum.valueToKey(netReplyP->error()));
        QMessageBox::warning(this,"Error",errorString,QMessageBox::Ok);

        return QByteArray();
    }

    if(!netReplyP->isOpen()){ // If canceled, skip reading from it
        return QByteArray();
    }

    return netReplyP->readAll();
}

std::tuple<QList<QStringList>,QString,bool> MainWindow::parseCourseData(const QByteArray &courseData){
    QJsonParseError parseError{};
    QJsonDocument jDoc=QJsonDocument::fromJson(courseData,&parseError);
    if(parseError.error!=QJsonParseError::NoError){
        return std::make_tuple(QList<QStringList>(),QString(),false);
    }

    QList<QStringList> courseList;
    for(const auto &courseValue:jDoc["records"].toArray()){
        QStringList course;

        for(const auto &attr:courseColumnAttrs){
            if(attr.columnTitle=="Location"){
                course.append(parseLocation(courseValue));
            }else if(attr.columnTitle=="Begin\nTime"||attr.columnTitle=="End\nTime"){
                course.append(parseTime(courseValue,attr.jsonName));
            }else{
                course.append(parseOthers(courseValue,attr));
            }
        }

        courseList.append(course);
    }

    QString currentTerm=courseList.front().at(
                std::find_if(
                    courseColumnAttrs.begin(),courseColumnAttrs.end(),
                    [](CourseColumnAttr &attr){return attr.columnTitle=="Current\nTerm";}
                )->columnIndex
            );
    QString term=courseList.front().at(
                std::find_if(
                    courseColumnAttrs.begin(),courseColumnAttrs.end(),
                    [](CourseColumnAttr &attr){return attr.columnTitle=="Term";}
                )->columnIndex
            );
    term[0]=term[0].toUpper();

    QString semester=QString("%1 %2").arg(term).arg(currentTerm.chopped(2));
    return std::make_tuple(courseList,semester,true);
}

QString MainWindow::parseLocation(const QJsonValue &courseValue){
    if(courseValue["building"]=="TBA"){
        return "(TBA)";
    }

    return QString("%1 %2").arg(
                courseValue["building"].toString(),
                courseValue["room"].toString());
}

QString MainWindow::parseTime(const QJsonValue &courseValue,const QString &jsonName){
    int timeNum=courseValue[jsonName].toInt();

    if(timeNum){
        QString timeString=QString("%1").arg(timeNum,4,10);
        timeString.insert(timeString.length()/2,":");
        return timeString;
    }

    return QString();
}

QString MainWindow::parseOthers(const QJsonValue &courseValue,const CourseColumnAttr &attr){
    auto value=courseValue[attr.jsonName];

    switch(attr.jsonType){
        case QJsonValue::Double:
            return QString::number(value.toInt());
        case QJsonValue::String:
            if(value.toString()=="TBA"){
                return "(TBA)";
            }
            return value.toString().replace("&#039;","'");
        default:
            return QString();
    }
}

void MainWindow::semesterButtonClicked(){
    // Find the attr belongs to the button
    auto button=dynamic_cast<QPushButton*>(sender());
    auto buttonName=button->objectName();
    auto attr=std::find_if(
        semesterButtonAttrs.begin(),semesterButtonAttrs.end(),
        [&buttonName](const SemesterButtonAttr &attr){return buttonName==attr.buttonName;}
    ).operator*();

    QByteArray courseData;
    if(secretCodeLineEdit->text()=="StaticCourseList"){
        courseData=getAll(attr.staticCourseListFileName);
    }else if(QGuiApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier)){
        auto fileName=QFileDialog::getOpenFileName(this,"Open File","","Json files (*.json)");
        courseData=getAll(fileName);
    }else{
        courseData=getAll(attr.dynamicCourseListUrl,attr.dynamicCourseListEstSize);
    }

    if(auto [courseList,semester,success]=parseCourseData(courseData);success){
        courseTableSetCourseList(courseList);
        courseTableColorizeEnroll();

        filterSetCourseList(courseList);
        groupBoxEnableAll();

        findChild<QLabel*>("semesterLabel")->setText(QString("Showing semester: ")+semester);
    }
}

void MainWindow::courseTableSetCourseList(const QList<QStringList> &courseList){
    courseTable->clearSelection();
    courseTable->clearContents();
    courseTable->setRowCount(courseList.size());

    for(int row=0;row<courseList.size();row++){
        for(const auto &attr:courseColumnAttrs){
            courseTable->setItem(
                row,attr.columnIndex,
                new QTableWidgetItem(courseList.at(row).at(attr.columnIndex))
            );
        }
    }
}

void MainWindow::courseTableColorizeEnroll(){
    const int actualEnrollColumnIndex=std::find_if(
                courseColumnAttrs.begin(),courseColumnAttrs.end(),
                [](CourseColumnAttr &attr){return attr.columnTitle=="Actual\nEnroll";}
            )->columnIndex;

    const int maxEnrollColumnIndex=std::find_if(
                courseColumnAttrs.begin(),courseColumnAttrs.end(),
                [](CourseColumnAttr &attr){return attr.columnTitle=="Max\nEnroll";}
            )->columnIndex;

    for(int row=0;row<courseTable->rowCount();row++){
        int actualEnroll=courseTable->item(row,actualEnrollColumnIndex)->text().toInt();
        int maxEnroll=courseTable->item(row,maxEnrollColumnIndex)->text().toInt();

        maxEnroll=std::max(maxEnroll,actualEnroll);

        if(maxEnroll){
            double percentEnroll=(double)actualEnroll/maxEnroll;
            auto color=QColor::fromHsvF((1-percentEnroll)*120/360,0.75,0.75);
            courseTable->item(row,actualEnrollColumnIndex)->setTextColor(color);
        }
    }
}

void MainWindow::selectCheckBoxStateChanged(){
    for(const auto &attr:courseColumnAttrs){
        courseTable->setColumnHidden(
            attr.columnIndex,
            attr.columnAlwaysHidden||!attr.selectCheckBox->isChecked()
        );
    }
}

void MainWindow::filterCheckBoxStateChanged(){
    auto checkBox=dynamic_cast<QCheckBox*>(sender());

    for(const auto &attr:filterAttrs){
        attr.comboBox->setDisabled(!attr.checkBox->isChecked());

        // Transfer focus to corresponding comboBox
        // No need to check for nullptr due to short-circuit evaluation
        if(checkBox==attr.checkBox&&checkBox->isChecked()){
            attr.comboBox->setFocus();
        }
    }

    // Invoke method after this function ends
    // Qt::QueuedConnection is necessary to invoke AFTER this function ends
    QMetaObject::invokeMethod(this,&MainWindow::filterComboBoxCurrentTextChanged,Qt::QueuedConnection);
}

void MainWindow::filterComboBoxCurrentTextChanged(){
    QVector<bool> hidden(courseTable->rowCount(),false);

    for(const auto &attr:filterAttrs){
        // Skip applying filter if not checked
        auto checkBox=attr.checkBox;
        if(!checkBox->isChecked()){
            continue;
        }

        // Skip applying filter if empty
        auto comboBox=attr.comboBox;
        auto comboBoxText=comboBox->currentText();
        if(comboBoxText.isEmpty()){
            continue;
        }

        QRegularExpression regExp(comboBoxText);

        for(int row=0;row<courseTable->rowCount();row++){
            auto courseText=courseTable->item(row,attr.columnIndex)->text();

            // skip if already hidden
            if(hidden.at(row)){
                continue;
            }

            // match regexp
            if(settings["useRegExpInFilter"]==Qt::Checked&&regExp.isValid()){
                hidden[row]=!regExp.match(courseText).hasMatch();
                continue;
            }

            // match full text
            if(attr.matchFullText&&courseText.compare(comboBoxText,attr.caseSensitivity)!=0){
                hidden[row]=true;
                continue;
            }

            // match partial text
            if(!attr.matchFullText&&!courseText.contains(comboBoxText,attr.caseSensitivity)){
                hidden[row]=true;
                continue;
            }
        }
    }

    for(int row=0;row<courseTable->rowCount();row++){
        courseTable->setRowHidden(row,hidden.at(row));
    }
}

void MainWindow::courseTableCurrentCellChanged(int currentRow,int currentColumn,int previousRow,int previousColumn){
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);

    // It could be -1 for the first call after loading course list
    if(currentRow==-1||currentColumn==-1){
        return;
    }

    for(const auto &attr:additionalInfoAttrs){
        attr.plainTextEdit->setPlainText(
            courseTable->item(currentRow,attr.columnIndex)->text()
        );
    }
}
