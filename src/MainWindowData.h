#ifndef MAINWINDOWDATA_H
#define MAINWINDOWDATA_H

#include<QJsonValue>
#include<QList>
#include<QCheckBox>
#include<QPushButton>
#include<QComboBox>
#include<QPlainTextEdit>

typedef struct CourseAttribute_{
    QString columnTitle;
    QString jsonName;
    QJsonValue::Type jsonType;
    int columnIndex;

    QString checkBoxName;
    QCheckBox *checkBox;

    int filterComboBoxSetIndex;
    bool columnAlwaysHidden;
    int columnWidth;

    bool operator<(const CourseAttribute_ &a)const{
        return columnIndex<a.columnIndex;
    }
}CourseAttribute;

static QList<CourseAttribute> courseAttributes{
    {"CRN","crn",QJsonValue::Double,0,"crnSelectCheckBox",nullptr,0,false,60},
    {"Subject","subject",QJsonValue::String,1,"subjectSelectCheckBox",nullptr,2,false,60},
    {"Course\nNumber","course_number",QJsonValue::String,2,"courseNumberSelectCheckBox",nullptr,1,false,60},
    {"Title","title",QJsonValue::String,3,"titleSelectCheckBox",nullptr,3,false,200},
    {"Instructor","instructor",QJsonValue::String,4,"instructorSelectCheckBox",nullptr,4,false,150},
    {"Days","days",QJsonValue::String,5,"daysSelectCheckBox",nullptr,5,false,60},
    {"Section","section",QJsonValue::String,6,"sectionSelectCheckBox",nullptr,-1,false,50},
    {"Begin\nTime","begin_time",QJsonValue::Double,7,"beginTimeSelectCheckBox",nullptr,-1,false,50},
    {"End\nTime","end_time",QJsonValue::Double,8,"endTimeSelectCheckBox",nullptr,-1,false,50},
    {"Actual\nEnroll","actual_enroll",QJsonValue::Double,9,"actualEnrollSelectCheckBox",nullptr,-1,false,50},
    {"Max\nEnroll","max_enroll",QJsonValue::Double,10,"maxEnrollSelectCheckBox",nullptr,-1,false,50},
    {"Description","description",QJsonValue::String,11,"",nullptr,-1,true,0},
    {"Credit\nHours","credit_hours",QJsonValue::Double,12,"",nullptr,-1,true,0},
    {"Location","",QJsonValue::Null,13,"",nullptr,-1,true,0},
    {"Current\nTerm","current_term",QJsonValue::String,14,"",nullptr,-1,true,0}
};

typedef struct{
    QString buttonName;
    QPushButton *button;

    QString staticCourseListFileName;
    QString dynamicCourseListUrl;
}SemesterButtonAttribute;

static QList<SemesterButtonAttribute> semesterButtonAttributes{
    {"springSemesterButton",nullptr,":/courseList/spring.json","http://api.fit.edu/courses/v1/courses?term=spring"},
    {"summerSemesterButton",nullptr,":/courseList/summer.json","http://api.fit.edu/courses/v1/courses?term=summer"},
    {"fallSemesterButton",nullptr,":/courseList/fall.json","http://api.fit.edu/courses/v1/courses?term=fall"}
};

typedef struct{
    QString checkBoxName;
    QCheckBox *checkBox;

    QString comboBoxName;
    QComboBox *comboBox;

    int columnIndex;
    int comboBoxSetIndex;
    Qt::CaseSensitivity caseSensitivity;
    bool matchFullText;
}FilterAttribute;

static QList<FilterAttribute> filterAttributes{
    {"crnFilterCheckBox",nullptr,"crnComboBox",nullptr,0,0,Qt::CaseInsensitive,true},
    {"courseNumberFilterCheckBox",nullptr,"courseNumberComboBox",nullptr,2,1,Qt::CaseInsensitive,false},
    {"subjectFilterCheckBox",nullptr,"subjectComboBox",nullptr,1,2,Qt::CaseInsensitive,true},
    {"titleFilterCheckBox",nullptr,"titleComboBox",nullptr,3,3,Qt::CaseInsensitive,false},
    {"instructorFilterCheckBox",nullptr,"instructorComboBox",nullptr,4,4,Qt::CaseInsensitive,true},
    {"daysFilterCheckBox",nullptr,"daysComboBox",nullptr,5,5,Qt::CaseInsensitive,true},
    {"descriptionFilterCheckBox",nullptr,"descriptionComboBox",nullptr,11,-1,Qt::CaseSensitive,false}
};

typedef struct{
    QString plainTextEditName;
    QPlainTextEdit *plainTextEdit;
    int columnIndex;
}AdditionalInfoAttribute;

static QList<AdditionalInfoAttribute> additionalInfoAttributes{
    {"descriptionPlainTextEdit",nullptr,11},
    {"creditHoursPlainTextEdit",nullptr,12},
    {"locationPlainTextEdit",nullptr,13},
};

typedef struct ElectiveAttribute_{
    QString name;
    QString key;

    bool operator<(const ElectiveAttribute_ &a)const{
        return key<a.key;
    }
}ElectiveAttribute;

static QList<ElectiveAttribute> electiveAttributes{
    {"",""},
    {"Humanities","(HU)"},
    {"Social Science","(SS)"},
    {"Liberal Arts","(LA)"},
    {"","(CL)"},
    {"","(Q)"},
    {"","(CC)"},
    {"Humanities/Social Science","(HU/SS)"},
    {"","(H)"}
};

#endif // MAINWINDOWDATA_H
