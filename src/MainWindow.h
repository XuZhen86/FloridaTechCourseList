#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QTableWidget>
#include"TechInfoDialog.h"

namespace Ui{
    class MainWindow;
}

class MainWindow:public QMainWindow{
    Q_OBJECT

    public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

    private:
    void findChild4Attributes();
    QList<QStringList> getCourseList(const QString &fileName);
    QList<QStringList> getCourseList(const QUrl &url);
    QList<QStringList> processJsonData(const QByteArray &jsonData);

    private:
    Ui::MainWindow *ui;
    TechInfoDialog *techInfoDialog;
    QTableWidget *courseTable;

    private slots:
    void semesterButtonClicked();
    void selectCheckBoxStateChanged();
    void filterCheckBoxStateChanged();
    void filterComboBoxCurrentTextChanged();
    void courseTableCurrentCellChanged(int currentRow,int currentColumn,int previousRow,int previousColumn);
};

#endif // MAINWINDOW_H
