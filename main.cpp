#include"mainwindow.h"

#include<QApplication>

int main(int argc,char *argv[]){
    // Make app aware of QHD resolutions
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication application(argc,argv);
    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
