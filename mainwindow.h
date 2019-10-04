#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>

#include<QList>
#include<QTableWidget>
#include<QJsonValue>
#include<QLabel>
#include"debugdialog.h"

namespace Ui{
    class MainWindow;
}

class MainWindow:public QMainWindow{
    Q_OBJECT

    public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow()override;

    private:
    QByteArray getAll(const QString &fileName);
    QByteArray getAll(const QUrl &url,const int estSize=-1);

    private:
    Ui::MainWindow *ui;
    QLineEdit *secretCodeLineEdit;
    QPushButton *debugButton;
    QLabel *currentVersionLabel;

    private:
    QTableWidget *courseTable;
    void courseTableInit();
    void courseTableSetCourseList(const QList<QStringList> &courseList);
    void courseTableColorizeEnroll();

    private:
    DebugDialog *debugDialog;
    void debugDialogInit();
    void debugDialogConnect();

    private:
    void attrsInit();
    struct CourseColumnAttr;
    static QList<CourseColumnAttr> courseColumnAttrs;
    void courseColumnAttrsInit();
    void courseColumnConnect();
    struct SemesterButtonAttr;
    static QList<SemesterButtonAttr> semesterButtonAttrs;
    void semesterButtonAttrsInit();
    void semesterButtonConnect();
    struct FilterAttr;
    static QList<FilterAttr> filterAttrs;
    void filterAttrsInit();
    void filterAttrsValidate();
    void filterAttrsConnect();
    void filterSetCourseList(const QList<QStringList> &courseList);
    struct AdditionalInfoAttr;
    static QList<AdditionalInfoAttr> additionalInfoAttrs;
    void additionalInfoAttrsInit();
    void additionalInfoAttrsValidate();
    struct ElectiveAttr;
    static QList<ElectiveAttr> electiveAttrs;
    void electiveAttrsInit();
    struct GroupBoxAttr;
    static QList<GroupBoxAttr> groupBoxAttrs;
    void groupBoxAttrsInit();
    void groupBoxEnableAll();

    private:
    std::tuple<QList<QStringList>,QString,bool> parseCourseData(const QByteArray &courseData);
    QString parseLocation(const QJsonValue &courseValue);
    QString parseTime(const QJsonValue &courseValue,const QString &jsonName);
    QString parseOthers(const QJsonValue &courseValue,const CourseColumnAttr &attr);

    private:
    QMap<QString,QVariant> settings;
    void advancedSettingInit();
    void advancedSettingConnect();

    private slots:
    void semesterButtonClicked();
    void selectCheckBoxStateChanged();
    void filterCheckBoxStateChanged();
    void filterComboBoxCurrentTextChanged();
    void courseTableCurrentCellChanged(int currentRow,int currentColumn,int previousRow,int previousColumn);
};

#endif // MAINWINDOW_H
