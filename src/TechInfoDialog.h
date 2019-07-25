#ifndef TECHINFODIALOG_H
#define TECHINFODIALOG_H

#include<QDialog>

namespace Ui{
    class TechInfoDialog;
}

class TechInfoDialog:public QDialog{
    Q_OBJECT

    public:
    explicit TechInfoDialog(QWidget *parent=nullptr);
    ~TechInfoDialog();

    private slots:
    void tabWidgetCurrentChanged(int index);
    void refreshPerformanceTab();

    private:
    Ui::TechInfoDialog *ui;
};

#endif // TECHINFODIALOG_H
