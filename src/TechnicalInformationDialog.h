#ifndef TECHNICALINFORMATIONDIALOG_H
#define TECHNICALINFORMATIONDIALOG_H

#include<QDialog>

namespace Ui{
    class TechnicalInformationDialog;
}

class TechnicalInformationDialog:public QDialog{
    Q_OBJECT

    public:
    explicit TechnicalInformationDialog(QWidget *parent=nullptr);
    ~TechnicalInformationDialog();

    private:
    Ui::TechnicalInformationDialog *ui;
};

#endif // TECHNICALINFORMATIONDIALOG_H
