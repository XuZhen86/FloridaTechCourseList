#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include<QDialog>
#include<QPlainTextEdit>

namespace Ui{
    class DebugDialog;
}

class DebugDialog:public QDialog{
    Q_OBJECT

    public:
    explicit DebugDialog(QWidget *parent=nullptr);
    ~DebugDialog() override;

    private:
    Ui::DebugDialog *ui;
    QPlainTextEdit *sysPlainTextEdit;

    private:
    struct SysInfoAttr;
    static QList<SysInfoAttr> sysInfoAttrs;
    void sysInfoAttrsInit();
};

#endif // DEBUGDIALOG_H
