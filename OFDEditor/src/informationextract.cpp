#include "informationextract.h"
#include "thread_getpagetext.h"

int InformationExtract::imageNum = 0;
InformationExtract::InformationExtract(QObject* parent): QObject(parent)
{

}

void InformationExtract::RichTextToOFD(QVector<Paper*> paperContain, QString f)
{
    //PageInfo pageInfo;
    filename = f;
    pageNum = paperContain.size();

    //����һ��ҳ�����Ϣ
    int i = 0;
    for(QVector<Paper*>::iterator it = paperContain.begin(); it!=paperContain.end(); it++, i++)
    {
        //���ö��߳���ȡҳ������
       Thread_GetPageText *thread = new Thread_GetPageText(*it, i);
       connect(thread, SIGNAL(hasDone(PageInfo)),
                        this, SLOT(receiveThread(PageInfo)));
       thread->start();
    }
//   writeToO(filename);                      //�������󣬲�����д�롣
}

//�������е�ҳ���������
bool InformationExtract::cmp(const PageInfo &a, const PageInfo &b)
{
    return a.number < b.number;
}

void InformationExtract::receiveThread(PageInfo pageInfo)
{
    qDebug() << "���յ����̵߳��ź�";
    qDebug() << "ҳ�Ž���"<< pageInfo.number;
    qDebug() << pageInfo.pageMargin.leftMargin;
    //ȡ�õ���ҳ����Ϣ
//    pageInfo.lineDistance = lineDistance;    //ȡ���оࡣ�ܱʣ��������޸�
    pageInfoContain.append(pageInfo);

    //��ҳ����ȡ��ϣ�����ofdprocess����
    if(pageInfoContain.size() == pageNum)
    {
        //���̶߳�ȡҳ����ٶȲ�һ���������ｫҳ�水��ҳ����������
        std::sort(pageInfoContain.begin(), pageInfoContain.end(), cmp);
        writeToOFD(filename);                      //����ofd���󣬲�����д�롣
    }

}

//�ĵ��Ľ���
PageInfo InformationExtract::getInfoFromRichText(Paper* paper)
{
    PageInfo pageInfo;
    pageInfo.pageHeight = paper->getHeight();
    pageInfo.pageWidth = paper->getWidth();
    pageInfo.lineDistance = paper->getLineDistance();
    pageInfo.letterDistanceScale = paper->getLetterDistanceScale();

    QTextFrame *rootFrame = paper->document()->rootFrame();
    QTextTable *table;
    QVector<BlockInfo> blockInfoContain;
    QTextBlock block;

    //�õ������Ϣ,��������ҳ�߾�
    QTextFrameFormat frameFormat = rootFrame->frameFormat();
    pageInfo.pageMargin.leftMargin = frameFormat.leftMargin();
    pageInfo.pageMargin.rightMargin = frameFormat.rightMargin();
    pageInfo.pageMargin.topMargin = frameFormat.topMargin();
    pageInfo.pageMargin.bottomMargin = frameFormat.bottomMargin();

    //����һ����ܣ�����ܣ�������ȡ�����е��ı���
    for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
    {
        table = (QTextTable*)it.currentFrame();
        block = it.currentBlock();
        QVector<FragmentInfo> fragmentInfoContain;

        //ȡ���ı����ʽ
        QTextBlockFormat blockFormat = block.blockFormat();
        BlockInfo blockInfo;

        //�õ��Ķ���0����������������������Ϊû������margin������ж��ǿո��ַ�������
        blockInfo.leftMargin = blockFormat.leftMargin();
        blockInfo.rightMargin = blockFormat.rightMargin();
        blockInfo.topMargin = blockFormat.topMargin();
        blockInfo.bottomMargin = blockFormat.bottomMargin();
        blockInfo.lineHeight = blockFormat.lineHeight();
        blockInfo.indent = blockFormat.indent();              //����������ı�д���ܻ�����

        if(!block.text().isEmpty()&&!table)
        {
             //�����ı����е��ı�Ƭ��,�ı������Ҫ�õ�����Ϣ
            for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
            {
                //�ж��Ƿ���ͼƬ��Դ
                QTextFragment fragment = it.fragment();
                FragmentInfo fragmentInfo;
                QTextCharFormat charFmt = fragment.charFormat();
                QTextImageFormat imageFmt = fragment.charFormat().toImageFormat();

                //����QTextEdit����������ת��,��������ı���ʽΪ��
                if(charFmt.fontFamily().isEmpty())
                {
                    charFmt.setFontFamily("����");
                    charFmt.setFontPointSize(11);
                }

                if(imageFmt.isValid()) {
                    //��ȡͼƬ
                    fragmentInfo.isImage = true;
                    fragmentInfo.isTable = false;         
                    qDebug() << imageFmt.name();
                    QString imagePath = imageFmt.name().right(imageFmt.name().size()-7);
                    // /*windows�ļ���ʽ,linuxɾ��
                    imagePath.replace(1,1,':');
                    imagePath.insert(2,'/');
                    //  */
                    qDebug() << imagePath;
                    fragmentInfo.imagePath = imagePath;

                    fragmentInfo.imageNum = imageNum;   //ͼƬID����
                    fragmentInfo.height = imageFmt.height();
                    fragmentInfo.width = imageFmt.width();
                    imageNum++;
                    qDebug() << "����һ��ͼƬ";
                }
                else
                {
                    fragmentInfo.fontFamily = charFmt.fontFamily();       //��ȡ����
                    fragmentInfo.size = charFmt.fontPointSize();          //��ȡ�ֺ�
                    //�õ��ı��θ�ʽ
                    fragmentInfo.isTable = false;
                    fragmentInfo.isImage = false;                         //����ͼƬ     
                    fragmentInfo.italic = charFmt.fontItalic();           //�Ƿ�б��
                    fragmentInfo.underLine = charFmt.fontUnderline();     //�Ƿ��»���
                    fragmentInfo.bold = charFmt.fontWeight();             //�Ƿ�Ӵ�
                    fragmentInfo.color = convertColor(charFmt.foreground().color().red(),charFmt.foreground().color().green(),charFmt.foreground().color().blue());
                    fragmentInfo.text = fragment.text();                  //�ı�����
                    qDebug() << "ȡ��blockInfo���ı��� " << fragmentInfo.text;
                    qDebug() << "block�ı���ʽ" << charFmt.fontFamily();
                    qDebug() << "�����С" << fragmentInfo.size;
                }
                fragmentInfoContain.append(fragmentInfo);
                blockInfo.empty = false;
                blockInfo.fragmentInfoContain = fragmentInfoContain;
            }
         }
         else if(block.text().isEmpty()&&table)
         {
            QTextTableFormat tableFormat = table->format();
            QVector<QTextLength> widthes = tableFormat.columnWidthConstraints();
            QTextTableCell cell;
            FragmentInfo fragmentInfo;

            for(int i=0; i<table->rows(); i++)
                for(int j=0; j<table->columns(); j++)
                {
                    cell = table->cellAt(i, j);
                    //���յ�Ԫ��
                    if(cell.begin().currentBlock().begin().atEnd())
                    {
                        fragmentInfo.isImage = false;
                        fragmentInfo.isTable = true;
                        fragmentInfo.text.clear();                           //������
                        fragmentInfo.cellWidth = widthes[j].rawValue();      //�����
                        fragmentInfo.cellPosition.col = j;                   //����λ��
                        fragmentInfo.cellPosition.row = i;
                        fragmentInfoContain.append(fragmentInfo);
                        blockInfo.empty = false;
                        blockInfo.fragmentInfoContain = fragmentInfoContain;
                        qDebug() << "����һ���յ�Ԫ��";
                    }

                    //����ڳ�ʼ������ȫ������һ�£������ｫ���ʼ��
                    for(QTextBlock::iterator it = cell.begin().currentBlock().begin(); !it.atEnd(); it++)
                    {
                        QTextFragment fragment = it.fragment();               
                        fragmentInfo.isTable = true;
                        QTextCharFormat charFmt = fragment.charFormat();
                        //�õ�������ı��θ�ʽ
                        fragmentInfo.isImage = false;                         //����ͼƬ
                        fragmentInfo.fontFamily = charFmt.fontFamily();       //��ȡ����
                        fragmentInfo.size = charFmt.fontPointSize();          //��ȡ�ֺ�
                        fragmentInfo.italic = charFmt.fontItalic();           //�Ƿ�б��
                        fragmentInfo.underLine = charFmt.fontUnderline();     //�Ƿ��»���
                        fragmentInfo.bold = charFmt.fontWeight();             //�Ƿ�Ӵ�
                        fragmentInfo.color = convertColor(charFmt.foreground().color().red(),charFmt.foreground().color().green(),
                                                          charFmt.foreground().color().blue());
                        fragmentInfo.text = fragment.text();     //�ı�����
                        fragmentInfo.cellWidth = widthes[j].rawValue();      //�����
                        fragmentInfo.cellPosition.col = j;                   //����λ��
                        fragmentInfo.cellPosition.row = i;
                        fragmentInfoContain.append(fragmentInfo);
                        blockInfo.empty = false;
                        blockInfo.fragmentInfoContain = fragmentInfoContain;
                    }
                }
         }
         else
         {
            blockInfo.empty = true;
            qDebug() << "����һ����block";
         }
         blockInfoContain.append(blockInfo);
         block = block.next();
    }
    pageInfo.blockInfoContain = blockInfoContain;
    return pageInfo;
}

//�����ĵ�
void InformationExtract::writeToOFD(QString filename)
{
    OFDProcess *ofd = new OFDProcess(filename);
    ofd->createOFD(pageInfoContain);
    this->deleteLater();
}

QString InformationExtract::convertColor(int x, int y, int z)
{
    return QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z));
}

