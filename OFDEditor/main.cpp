#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	//����ȫ��������ʽ
	QFont font;
	font.setPointSize(11);
	font.setFamily(QStringLiteral("����"));
	a.setFont(font);
    MainWindow w;
    w.show();
    return a.exec();
}
