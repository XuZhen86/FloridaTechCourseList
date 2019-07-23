#include<QApplication>

#include"MainWindow.h"
#include"version/Version.h"

int main(int argc,char *argv[]){
    QApplication::setApplicationVersion(versionString);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication application(argc,argv);
    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
