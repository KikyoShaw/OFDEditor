#include "richtextprocess.h"
#include "thread_loadrichtext.h"

RichTextProcess::RichTextProcess(QWidget *parent) : QWidget(parent)
{
    paperNum = 1;
    //ֽ��ʵ�ʴ�С
    height = A4height;
    width = A4width;
    scale = 1;

    mainLayout = new QVBoxLayout(this);
    Paper *paper = new Paper(this);
    mainLayout->addWidget(paper);
    mainLayout->setMargin(defaultMargin*scale);
    mainLayout->setSpacing(defaultSpace*scale);              //���Ÿ�ֽ֮������ף�������Ϊ0��������editor�Ĵ�СʱӦ�ü��㵽
    paperContain.append(paper);                        //��������
}

int RichTextProcess::getPaperNum()
{
    return paperNum;
}

Paper* RichTextProcess::getCurrentPaper()
{
    Paper* paper = NULL;
    QWidget *current = this->focusWidget();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if((*it) == current)
            paper = (*it);
    }
    return paper;
}

//�������ָ�ʽ
void RichTextProcess::setFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = this->getCurrentPaper()->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    this->getCurrentPaper()->mergeCurrentCharFormat(format);
}

//��ʽˢ����
void RichTextProcess::formatPainter(bool checked)
{
    QTextCursor textCursor = this->getCurrentPaper()->textCursor();
    if(checked)
        painterSelected = textCursor.charFormat();
    else
    {
        textCursor.mergeCharFormat(painterSelected);
        this->getCurrentPaper()->mergeCurrentCharFormat(painterSelected);
    }

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if(checked)
            (*it)->viewport()->setCursor(QPixmap(":resources/format_mouse.png"));    //ð��ָ����ԴĿ¼
        else
            (*it)->viewport()->setCursor(Qt::IBeamCursor);
    }
}

//����ҳ��
void RichTextProcess::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("���Ϊ"), QStringLiteral("�½�OFD�ĵ�.ofd"), QStringLiteral("OFD �ĵ�"));
    if(filename.isEmpty())
        return;
    return saveFile(filename);
}

//���ڱ���richText������ʹ��
bool RichTextProcess:: saveToRichText()
{
    int i=0;
    //����ļ���
    QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("���༭�ļ�"), QStringLiteral("�½�OFDX�ļ�.ofdx"), QStringLiteral("OFDX �ĵ�"));
    if (!(filename.endsWith(".ofdx", Qt::CaseInsensitive))) {
        filename += ".ofdx";
    }

    QZipWriter zip_writer(filename);

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        QFile file(QString("paper_%1.html").arg(QString::number(i)));
        if(!file.open(QFile::ReadWrite)) return false;
        QTextDocumentWriter writer;
        writer.setDevice(&file);
        writer.write((*it)->document());
        file.close();

        //��ӵ�ofdx
        if(!file.open(QFile::ReadWrite)) return false;    //������fileҪ���´򿪣���Ϊ��writerռ�����Ի�����ļ���ʧ��
        zip_writer.addFile(QString("paper_%1.html").arg(QString::number(i)), file.readAll());
        file.close();
        QFile::remove(QString("paper_%1.html").arg(QString::number(i)));
        i++;
    }
    //д�������ļ�������ҳ�淽��ҳ���С��Ϣ��
    QFile file("pageConfigure.txt");
    if(!file.open(QFile::ReadWrite)) return false;
    QTextStream text;
    text.setDevice(&file);
    text << "height:" << height << endl << "width:" << width << endl;
    qDebug() << "height:" << height << "width:" << width;
    text << "letterDistanceScale:" << endl;

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        text << (*it)->getLetterDistanceScale() << endl;
    }
    file.close();

    //��ӵ�ofdx
    if(!file.open(QFile::ReadWrite)) return false;    //������fileҪ���´򿪣���Ϊ��writerռ�����Ի�����ļ���ʧ��
    zip_writer.addFile("pageConfigure.txt", file.readAll());
    file.close();
    QFile::remove("pageConfigure.txt");

    zip_writer.close();
    return true;
}

bool RichTextProcess::editorIsEmpty()
{
    bool editorIsEmpty = true;
    //�ж��ı��༭���Ƿ������ݣ�����������ʾ���棬�����ý���
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if(!(*it)->document()->isEmpty())
        {
            editorIsEmpty = false;
            break;
        }
    }
    return editorIsEmpty;
}

//�������߳����еĺ���,����ҳ��
QString RichTextProcess::extractRichText(int i)
{
    qDebug() << "��ȡ��ҳ���ı��ĵ�" << i;
    QFile file;
    file.setFileName("./paper_"+QString::number(i)+".html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.fileName();
    }
    //����html������༭��
    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    file.close();
    return str;
}

bool RichTextProcess::cmp(Paper* a, Paper* b)
{
    return a->getPaperNum() - b->getPaperNum();
}

void RichTextProcess::receiveLoadRichText(QString str, Paper* paper)
{
  //  htmlTmp.append(str);
    paper->setHtml(str);
    //qDebug() << "������ҳ��" << paper <<paper->getPaperNum();
    count++;
    if(count==paperContain.size())       //��֤�������
    {
        QDir dir;
        //ɾ����ʱ�ļ���ofdxing
        dir.setCurrent("../"); //�ļ�Ŀ¼���õ���һ��
        OFDProcess::DeleteDirectory("ofdxing");

        //��ҳ�����ݼ�����֮����������־�
        QVector<double>::iterator l = letterDistanceScaleContain.begin();
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
        {
            (*it)->setLetterDistanceScale(*l);
            qDebug() << "ÿҳ���־�"<<*l;
            l++;
        }
        letterDistanceScaleContain.clear();
    }
}

//��ofdx�ļ�
bool RichTextProcess::openRichText()
{
    htmlTmp.clear();           //�������ʱ����
    QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ��OFDX�ĵ�"), ".", tr("OFDX (*.ofdx *.ofd)"));
    if(filename.isEmpty())
       return false;

	//��ȡ�ļ���
	QString fileText = filename;
	if (filename.contains("/"))
	{
		QStringList list = fileText.split("/");
		fileText = list[list.size() - 1];
	}

	fileText = QStringLiteral("./ofdxing/%1").arg(fileText);

    //��ѹofdx�ļ�
    QDir dir;
    dir.mkdir(fileText);
    QZipReader reader(filename);
    reader.extractAll(fileText);

	/*QFile file1(filename);
	file1.open(QIODevice::WriteOnly);
	file1.write(reader.fileData(QString::fromLocal8Bit("%1").arg(filename)));//��ѹ�ļ�
	file1.close();
	reader.close();*/

    //����editor
    resetEditor();

    //�����ļ��У�ȡ�������ļ�
    dir.setCurrent(fileText);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    //����Ϊ�����ļ�������ֽ����Ϊ���ڽ���û�п��ƺã�����ʹ������ѭ����Щ������
    for(int i=0; i < list.size()-2; ++i)    //ȥ�������ļ��͵�һҳ
    {
        createNewPaper();
        resizePage();
    }

    //�ļ���ҳ������һ��������ļ����ص���������
    QVector<Paper*>::iterator it = paperContain.begin();
    count = 0;             //ҳ�����
    for (int i = 0; i < list.size()-1; ++i, ++it)     //ȥ�������ļ�
    {

        //���̼߳���ҳ��
        Thread_loadRichText *thread = new Thread_loadRichText(i, list, (*it));
        connect(thread, SIGNAL(hasDone(QString, Paper*)), this, SLOT(receiveLoadRichText(QString, Paper*)));
        (*it)->setPaperNum(i);
        qDebug() << "ҳ���"<<(*it)->getPaperNum();
        thread->start();
    }

    //����pageConfigure.txt
    //Qt���ļ�·���е����
    QFile file;
    file.setFileName("." + fileText + "/pageConfigure.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() <<"���ļ�����";
        return false;
    }
    QTextStream text;
    text.setDevice(&file);

    bool flag = false;
    while(!text.atEnd())
    {
        QString info =  text.readLine();
        qDebug() << info;
        //����������Ϣ���ַ���
        if(info.contains("height"))
            height = info.right(info.size()-7).toInt();
        if(info.contains("width"))
            width = info.right(info.size()-6).toInt();
        if(flag == true)
            letterDistanceScaleContain.append(info.toDouble());
        if(info=="letterDistanceScale:")
            flag = true;
    }

    setPaperHeight(height);
    setPaperWidth(width);
    qDebug() << "��ҳ�棺height:" << height << "\n" << "width:" << width;
    resizePage();
    file.close();
    return true;
}

void RichTextProcess::resetEditor()
{
    //ɾ�����е�ҳ�棬�����һ��ҳ��
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        mainLayout->removeWidget((*it)); //ж�ظ�ֽ
        (*it)->deleteLater();               //���ڴ�
        paperNum--;
    }
    paperContain.clear();                //������
    createNewPaper();                    //��ӵ�һ�Ÿ�ֽ
    resizePage();
}

void RichTextProcess::setAlign(int i)
{
    switch(i)
    {
    case 0:
        this->getCurrentPaper()->setAlignment(Qt::AlignLeft|Qt::AlignAbsolute);
        break;
    case 1:
        this->getCurrentPaper()->setAlignment(Qt::AlignRight|Qt::AlignAbsolute);
        break;
    case 2:
        this->getCurrentPaper()->setAlignment(Qt::AlignCenter);
        break;
    case 3:
        this->getCurrentPaper()->setAlignment(Qt::AlignJustify);
        break;
    }
}

//�½�ҳ��
void RichTextProcess::createNewPaper()
{
    Paper *paper = new Paper(this);
    paperContain.append(paper);                    //��ҳ����뵽��������
    mainLayout->addWidget(paper);
    paperNum++;
    emit paperNumChanged(paperNum);
}

void RichTextProcess::deleteLastPaper()
{
    //��Ҫ�ж��ǲ��ǵ�һҳ
    Paper *lastPaper = paperContain.last();
    if(paperContain.first() == paperContain.last())
        return;
    mainLayout->removeWidget(lastPaper);  //ж�ظÿؼ�

    //QT 5
    //paperContain.removeLast();          //������ɾ��
    lastPaper->deleteLater();             //���ڴ�
    paperContain.removeLast();            //ɾ��ַ
    paperNum--;
    paperContain.last()->setFocus();
    emit paperNumChanged(paperNum);
}

void RichTextProcess::setPaperHeight(double h)
{
    height = h;
}

void RichTextProcess::setPaperWidth(double w)
{
    width = w;
}

double RichTextProcess::getPaperHeight()
{
    return height;
}

double RichTextProcess::getPaperWidth()
{
    return width;
}

double RichTextProcess::calculateTotalHeight()
{
    return height*paperNum+2*defaultMargin+(paperNum-1)*defaultSpace;
}

void RichTextProcess::resizePage()
{
    //������ÿ��ҳ��ĸ߶ȺͿ��
    //����ÿһҳ����Ҫ��ȡҳ���С����������ҳ���С���Ǽ������
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->setHeight(height);
        (*it)->setWidth(width);
    }
    mainLayout->setMargin(defaultMargin*scale);
    mainLayout->setSpacing(defaultSpace*scale);
    resize(width*scale,calculateTotalHeight()*scale);
}

void RichTextProcess::saveFile(QString filename)
{
    informationExtract = new InformationExtract;
    informationExtract->RichTextToOFD(paperContain, filename);        //���ת���������һϵ�й���
}

void RichTextProcess::setlineDistance(double h, bool all)
{
    if(all)
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
            (*it)->setLineDistance(h);
    else
        this->getCurrentPaper()->setLineDistance(h);
}

void RichTextProcess::setLetterDistanceScale(double l, bool all)
{
    if(all)
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
            (*it)->setLetterDistanceScale(l);
    else
        this->getCurrentPaper()->setLetterDistanceScale(l);
}

//���ͼƬ����
void RichTextProcess::insertImage(QString imagePath, double height, double width)
{
    Paper* paper = getCurrentPaper();
    //����Paper���崦��
    paper->insertImage(imagePath, height, width);
}

//��ӱ����
void RichTextProcess::insertTable(double row, double col, double width)
{
    Paper* paper = this->getCurrentPaper();
    QTextCursor cur(paper->textCursor());
    QTextLength len(QTextLength::FixedLength, width);
    QVector<QTextLength> constaint(col,len);

    QTextTableFormat tableFormat;
    tableFormat.setCellSpacing(1);
    tableFormat.setColumnWidthConstraints(constaint);
    QTextTable* table = cur.insertTable(row, col, tableFormat);


    //���ñ���ʼ�����ʽ
    QTextCharFormat format;
    format.setFontFamily("����");
    format.setFontPointSize(11);
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
        {
            QTextTableCell cell = table->cellAt(i, j);
            cell.setFormat(format);
        }
}

QMessageBox* RichTextProcess::showMessage()
{
    QMessageBox* msgBox = new QMessageBox();
    msgBox->setText(QStringLiteral("�༭����������"));
    msgBox->setInformativeText(QStringLiteral("�Ƿ񱣴�δOFDX�ĵ�?"));
    msgBox->setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox->setButtonText (QMessageBox::Save, QStringLiteral("�� ��"));
    msgBox->setButtonText (QMessageBox::Discard, QStringLiteral("������"));
    msgBox->setButtonText (QMessageBox::Cancel, QStringLiteral("ȡ ��"));
    return msgBox;
}

void RichTextProcess::setPaperMarginToAll(double leftMargin, double rightMargin, double topMargin,double bottomMargin)
{
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->setLeftMargin(leftMargin);
        (*it)->setRightMargin(rightMargin);
        (*it)->setTopMargin(topMargin);
        (*it)->setBottomMargin(bottomMargin);
    }
}

void RichTextProcess::changePaperDirection()
{
    double tmp = getPaperHeight();
    setPaperHeight(getPaperWidth());
    setPaperWidth(tmp);
    resizePage();
}

double RichTextProcess::getScale()
{
    return scale;
}

void RichTextProcess::enlarge()
{
    scale = 1.5;
 //   resizePage();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->zoomIn(1.5);
        qDebug() << "���ڱ������С";
    }

}

void RichTextProcess::shrink()
{
    scale = 0.5;
    //�����ı����������С
    resizePage();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        /*
        //�ı������С
        QTextFrame *rootFrame = (*it)->document()->rootFrame();
        for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
        {
            QTextBlock block = it.currentBlock();
            for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
            {
                QTextFragment fragment = it.fragment();
                QTextCharFormat charFmt = fragment.charFormat();
                charFmt.setFontPointSize(charFmt.fontPointSize()*scale);
            }
        }*/
        ;

        (*it)->zoomIn(1);
        qDebug() << "�ı�";
    }
}

int RichTextProcess::countWords(){
    int length = 0;
    for(QVector<Paper*>::iterator it = paperContain.begin(); it!=paperContain.end(); it++)
    {
        Paper *paper = *it;
        QTextFrame *rootFrame = paper->document()->rootFrame();
        QTextBlock block;

        //����һ����ܣ�����ܣ�������ȡ�����е��ı���
        for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
        {
            block = it.currentBlock();

                 //�����ı����е��ı�Ƭ��,�ı������Ҫ�õ�����Ϣ
                for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
                {
                    //�ж��Ƿ���ͼƬ��Դ
                    QTextFragment fragment = it.fragment();

                    length += fragment.text().length();
                }
        }
    }
    qDebug() << "����ͳ��" << length;
    return length;
}

void RichTextProcess::preview(){
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void RichTextProcess::printPreview(QPrinter* printer){
    this->getCurrentPaper()->print(printer);
}



