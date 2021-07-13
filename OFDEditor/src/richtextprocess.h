#ifndef RICHTEXTPROCESS_H
#define RICHTEXTPROCESS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QTextTable>
#include <QTextDocumentWriter>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QFileDialog>
#include <QTextLength>
#include <QTextCodec>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QtGui/private/qzipwriter_p.h>
#include <QtGui/private/qzipreader_p.h>
#include "informationextract.h"
#include "paper.h"
#include <QtPrintSupport/qprinter.h>
#include <QtPrintSupport/QPrintPreviewDialog>

const double defaultMargin = 10.0;
const double defaultSpace = 10.0;

class RichTextProcess:public QWidget
{
    Q_OBJECT
public:
    explicit RichTextProcess(QWidget *parent = 0);

    int getPaperNum();
    Paper* getCurrentPaper();

    void setPaperHeight(double);
    void setPaperWidth(double);
    double getPaperHeight();
    double getPaperWidth();

    void setFormat(const QTextCharFormat&);
    void saveAs();
    void saveFile(QString);
    void setAlign(int);
    void createNewPaper();
    void deleteLastPaper();
    bool saveToRichText();
    double calculateTotalHeight();
    void resizePage();
    void setlineDistance(double, bool);
    void setLetterDistanceScale(double, bool);
    bool openRichText();
    void resetEditor();
    bool editorIsEmpty();
    static QString extractRichText(int);
    static QMessageBox* showMessage();
    static bool cmp(Paper*, Paper*);
    void setPaperMarginToAll(double, double, double, double);
    void formatPainter(bool);
    void enlarge();
    void shrink();
    double getScale();
    void changePaperDirection();
    void insertImage(QString, double, double);
    void insertTable(double, double, double);
    int countWords();
    void preview();

private:
    int paperNum;
    double height;
    double width;
    QVBoxLayout *mainLayout;
    QVector<Paper*> paperContain;
    InformationExtract *informationExtract;
    int count;                            //����ȷ�����̼߳���ҳ��ʱ�Ƿ�ȫ���������
    QVector<QString> htmlTmp;             //��ʱ��Ž�����htmlҳ�棬���������������
    QTextCharFormat painterSelected;      //���ڸ�ʽˢ��ȡ���ָ�ʽ
    double scale;                         //���ڱ༭����ʾʱ�������ţ���ʼ��Ϊ1
    QVector<double> letterDistanceScaleContain;    //���ڴ洢ÿһҳ���־�
public slots:
    void receiveLoadRichText(QString, Paper*);
    void printPreview(QPrinter*);
signals:
    void paperNumChanged(int);
};

#endif // RICHTEXTPROCESS_H
