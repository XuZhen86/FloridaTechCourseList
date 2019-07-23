#ifndef MAINWINDOWDATA_H
#define MAINWINDOWDATA_H

#include<QJsonValue>
#include<QList>

typedef struct CourseAttribute_{
    QString columnTitle;
    QString jsonName;
    QJsonValue::Type jsonType;
    int columnIndex;
    QString checkBoxObjName;
    int filterComboBoxSetIndex;
    bool columnAlwaysHidden;
    int columnWidth;

    bool operator<(const CourseAttribute_ &a)const{
        return columnIndex<a.columnIndex;
    }
}CourseAttribute;

static QList<CourseAttribute> courseAttributes{
    {"CRN","crn",QJsonValue::Double,0,"crnSelectCheckBox",0,false,60},
    {"Subject","subject",QJsonValue::String,1,"subjectSelectCheckBox",2,false,60},
    {"Course\nNumber","course_number",QJsonValue::String,2,"courseNumberSelectCheckBox",1,false,60},
    {"Title","title",QJsonValue::String,3,"titleSelectCheckBox",3,false,200},
    {"Instructor","instructor",QJsonValue::String,4,"instructorSelectCheckBox",4,false,150},
    {"Days","days",QJsonValue::String,5,"daysSelectCheckBox",5,false,60},
    {"Section","section",QJsonValue::String,6,"sectionSelectCheckBox",-1,false,50},
    {"Begin\nTime","begin_time",QJsonValue::Double,7,"beginTimeSelectCheckBox",-1,false,50},
    {"End\nTime","end_time",QJsonValue::Double,8,"endTimeSelectCheckBox",-1,false,50},
    {"Actual\nEnroll","actual_enroll",QJsonValue::Double,9,"actualEnrollSelectCheckBox",-1,false,50},
    {"Max\nEnroll","max_enroll",QJsonValue::Double,10,"maxEnrollSelectCheckBox",-1,false,50},
    {"Description","description",QJsonValue::String,11,"",-1,true,0},
    {"Credit\nHours","credit_hours",QJsonValue::Double,12,"",-1,true,0},
    {"Location","",QJsonValue::Null,13,"",-1,true,0},
    {"Current\nTerm","current_term",QJsonValue::String,14,"",-1,true,0}
};

typedef struct{
    QString buttonName;
    QString staticCourseListFileName;
    QString dynamicCourseListUrl;
}SemesterButtonAttribute;

static const QList<SemesterButtonAttribute> semesterButtonAttributes{
    {"springSemesterButton",":/courseList/spring.json","http://api.fit.edu/courses/v1/courses?term=spring"},
    {"summerSemesterButton",":/courseList/summer.json","http://api.fit.edu/courses/v1/courses?term=summer"},
    {"fallSemesterButton",":/courseList/fall.json","http://api.fit.edu/courses/v1/courses?term=fall"}
};

typedef struct{
    QString checkBoxName;
    QString comboBoxName;
    int columnIndex;
    int comboBoxSetIndex;
    Qt::CaseSensitivity caseSensitivity;
    bool matchFullText;
}FilterAttribute;

static const QList<FilterAttribute> filterAttributes{
    {"crnFilterCheckBox","crnComboBox",0,0,Qt::CaseInsensitive,true},
    {"courseNumberFilterCheckBox","courseNumberComboBox",2,1,Qt::CaseInsensitive,false},
    {"subjectFilterCheckBox","subjectComboBox",1,2,Qt::CaseInsensitive,true},
    {"titleFilterCheckBox","titleComboBox",3,3,Qt::CaseInsensitive,false},
    {"instructorFilterCheckBox","instructorComboBox",4,4,Qt::CaseInsensitive,true},
    {"daysFilterCheckBox","daysComboBox",5,5,Qt::CaseInsensitive,true},
    {"descriptionFilterCheckBox","descriptionComboBox",11,-1,Qt::CaseSensitive,false}
};

typedef struct{
    QString plainTextEditName;
    int columnIndex;
}AdditionalInfoAttribute;

static const QList<AdditionalInfoAttribute> additionalInfoAttributes{
    {"descriptionPlainTextEdit",11},
    {"creditHoursPlainTextEdit",12},
    {"locationPlainTextEdit",13},
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
