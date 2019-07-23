#include<QApplication>
#include"MainWindow.h"

int main(int argc,char *argv[]){
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication application(argc,argv);
    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
