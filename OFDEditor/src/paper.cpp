#include "paper.h"

Paper::Paper(QWidget *parent):QTextEdit(parent)
{
    initPaper();
}

void Paper::initPaper()
{
    this->activateWindow();
    this->setFocus();
//    repaint();
    height = A4height;
    width = A4width;
    paperMargin = {30.0,30.0,30.0,30.0};
    lineDistance = 3.5;
    letterDistanceScale = 1.0;

    //�����־�
    QTextCharFormat charFmt;
    charFmt.setFontFamily(QStringLiteral("����"));
    charFmt.setFontPointSize(11);
    charFmt.setFontLetterSpacing(letterDistanceScale*100);
    this->setCurrentCharFormat(charFmt);

    //���û�������
    QFont font;
    font.setPointSize(11);
    this->setFont(font);

    doc = this->document();
    rootFrame = doc->rootFrame();
    QTextFrameFormat frameFormat;
    frameFormat.setLeftMargin(paperMargin.leftMargin);
    frameFormat.setRightMargin(paperMargin.rightMargin);
    frameFormat.setTopMargin(paperMargin.topMargin);
    frameFormat.setBottomMargin(paperMargin.bottomMargin);
    rootFrame->setFrameFormat(frameFormat);

    connect(this, SIGNAL(textChanged()), this, SLOT(ifIsEmpty()));
    connect(this, SIGNAL(textEditIsEmpty()), this, SLOT(resetPaper()));   //��ֹ����Ϊ�պ�textEdit���ûص�Ĭ��ֵ������Ϊһ���жϺ���
}

void Paper::setLeftMargin(double num)
{
    paperMargin.leftMargin = num;
    setFrameMargin();
}

void Paper::setRightMargin(double num)
{
    paperMargin.rightMargin = num;
    setFrameMargin();
}

void Paper::setTopMargin(double num)
{
    paperMargin.topMargin = num;
    setFrameMargin();
}

void Paper::setBottomMargin(double num)
{
    paperMargin.bottomMargin = num;
    setFrameMargin();
}

void Paper::setPaperMargin(double l, double r, double t, double b)
{
    paperMargin.leftMargin = l;
    paperMargin.rightMargin = r;
    paperMargin.topMargin = t;
    paperMargin.bottomMargin = b;
}

void Paper::setLineDistance(double l)
{
    QTextBlockFormat fmt;
    fmt.setLineHeight(l, QTextBlockFormat::LineDistanceHeight);
    this->selectAll();
    QTextCursor cur = this->textCursor();
    cur.setBlockFormat(fmt);
    lineDistance = l;
}

void Paper::setLetterDistanceScale(double l)
{
    letterDistanceScale = l;
    this->selectAll();
    QTextCharFormat charFmt;
    charFmt.setFontLetterSpacing(letterDistanceScale*100);
    this->mergeCurrentCharFormat(charFmt);
}

double Paper::getLineDistance()
{
    return lineDistance;
}

PaperMargin Paper::getPaperMargin()
{
    return paperMargin;
}

void Paper::setFrameMargin()
{
    QTextFrameFormat frameFormat;
    frameFormat.setLeftMargin(paperMargin.leftMargin);                 //�����ÿ������ı߾඼��������ѽ����ȻframeFormat��֪��������
    frameFormat.setRightMargin(paperMargin.rightMargin);
    frameFormat.setTopMargin(paperMargin.topMargin);
    frameFormat.setBottomMargin(paperMargin.bottomMargin);
    rootFrame = doc->rootFrame();               //����򿪵�ҳ�治�ܽ���ҳ�����õ�bug
    rootFrame->setFrameFormat(frameFormat);
}

//���ͼƬ
void Paper::insertImage(QString imagePath, double height, double width)
{
    QImage image = QImageReader(imagePath).read();
    QUrl Uri(QString ( "file://%1" ).arg (imagePath));
    doc->addResource( QTextDocument::ImageResource, Uri, QVariant(image));
    QTextCursor cursor = this->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(width);
    imageFormat.setHeight(height);
    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);
}

void Paper::resetPaper()
{
    this->setFontFamily(QStringLiteral("����"));
    this->setFontPointSize(11);
}

void Paper::ifIsEmpty()
{
    if(this->toPlainText().isEmpty())
       emit textEditIsEmpty();
}

void Paper::setHeight(double h)
{
    height = h;
}

void Paper::setWidth(double w)
{
    width = w;
}

double Paper::getHeight()
{
    return height;
}

double Paper::getWidth()
{
    return width;
}

QTextDocument* Paper::getPaperDocument()
{
    return doc;
}

int Paper::getPaperNum()
{
    return num;
}

void Paper::setPaperNum(int i)
{
    num = i;
}

double Paper::getLetterDistanceScale()
{
    return letterDistanceScale;
}

