#include "thread_getpagetext.h"
#include "informationextract.h"

Thread_GetPageText::Thread_GetPageText(Paper* p, int i)
    :paper(p), number(i)
{
    //����ʶ��Ĳ����ṹ����ע�ᣬ��Qt�ܹ�ʶ��

    qRegisterMetaType<PageInfo>("PageInfo");//ע��PVCI_CAN_OBJ����
}

void Thread_GetPageText::run()
{
    pageInfo = InformationExtract::getInfoFromRichText(paper);
    pageInfo.number = number;
    qDebug() << "ҳ��"<< pageInfo.number;
    qDebug() << "�����߳���";
    emit hasDone(pageInfo);        //��ȡ��Ϣ��Ϻ��źŸ�InformationExtrct����
    this->quit();    //�����߳�
    this->deleteLater();   //ɾ������
}

PageInfo Thread_GetPageText::getPageInfo()
{
    return pageInfo;
}
