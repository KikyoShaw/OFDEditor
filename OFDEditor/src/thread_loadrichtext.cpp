#include "thread_loadrichtext.h"

Thread_loadRichText::Thread_loadRichText(int n, QFileInfoList f, Paper* p):
    num(n), list(f), paper(p)
{

}

void Thread_loadRichText::run()
{
    qDebug() << "���ڼ��ظ��ı����߳���";
    QString str = RichTextProcess::extractRichText(num);
    emit hasDone(str, paper);
    this->quit();
    this->deleteLater();
}
