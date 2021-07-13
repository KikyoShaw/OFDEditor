#ifndef OFDPROCESS_H
#define OFDPROCESS_H

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDir>
#include <QString>
#include <QUuid>
#include <QMessageBox>
#include <QtGui\private\qzipwriter_p.h>
#include <QMainWindow>
#include <QTextDocument>
#include <QVector>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextLength>
#include <QDebug>

//ҳ���С
#define A4page_page_PhysicalBox "0 0 210 297"
#define A4Tpage_page_PhysicalBox "0 0 297 210"
#define A5page_page_PhysicalBox "0 0 148 210"
#define A5Tpage_page_PhysicalBox "0 0 210 148"
#define B5page_page_PhysicalBox "0 0 210 276"
#define B5Tpage_page_PhysicalBox "0 0 276 210"

const double CTFont = 0.343;           //��ʾ��С�������ofd����֮���ϵ��
const double CTMargin = 0.2058;           //��ʾ��С�������ofdmargin֮���ϵ��
const double CTLetter = 0.531;          //Ӣ���ַ���ƫ�ƾ���ϵ��
const double CTView = 0.2;             //paper��ʾ�����õ����ű���
const double CTLineDistance = 0.22;
const double CTImage = 0.2;             //ͼƬ��ʾϵ��
const double CTTable = 0.215;            //�����ת������
const double BaseLine = 1;       //��ֹ�������·��Ϸ�������һ��

typedef struct CellPosition
{
    int row;
    int col;
}CellPosition;

typedef struct FragmentInfo
{
    QString fontFamily;
    double size;
    bool underLine;
    int bold;
    bool italic;
    QString color;
    QString text;

    bool isImage;
    QString imagePath;
    int imageNum;
    double height;
    double width;

    bool isTable;
    CellPosition cellPosition;
    double cellWidth;
    double celleHeight;
    int tablePosition;
}Fragment;

typedef struct BlockInfo
{
    double leftMargin;     //blockInfo���margin�����������ö�����Ϣ
    double rightMargin;
    double topMargin;
    double bottomMargin;
    double lineHeight;
    int indent;             //����
    bool empty;             //ȷ���ı����Ƿ�Ϊ�գ�Ϊ���д��»���
    QVector<FragmentInfo> fragmentInfoContain;
}BlockInfo;

//�����Ϣ
typedef struct TableInfo
{

}TableInfo;

typedef struct pageMargin
{
    double leftMargin;
    double rightMargin;
    double bottomMargin;
    double topMargin;
}PageMargin;

//QDocument�ı���Ϣ�ṹ��
typedef struct PageInfo
{
    int number;
    double pageHeight;
    double pageWidth;
    double lineDistance;        //�о�
    double letterDistanceScale;  //�־����
    PageMargin pageMargin;
    QVector<BlockInfo> blockInfoContain;
    QVector<TableInfo> tableInfoContain;
}PageInfo;

//OFD�ĵ���Ϣ�ṹ��
typedef struct DocInfo
{
    QString title;                        //�ĵ�����
    QString author;                       //�ĵ�����
    QString creator;                      //�༭��
    QString creatorVersion;               //�༭���汾
    QUuid uuid;                           //ȫ��Ψһ��ʶ��uuid
}DocInfo;

//�ı���Ϣ�ṹ�� �����ַ������ͣ�Ϊ��XML��д��
typedef struct Boundary
{
    double x;
    double y;
    double dx;
    double dy;
    QString str;
}Boudary;

typedef struct TextInfo
{
    QString type;                         //����
    QString fontID;                       //���壬ֵΪPublicRes.xml���������ID��
    QString CTM;                          //CTM����任
    Boundary boundary;                     //�߽�
    QString size;                         //�����С
    double dxScale;
 //   QTextDocument text;
    QString X;
    QString Y;
    QString DeltaX;
    QString text;

    QString underLine;
    QString italic;
    QString bold;
    QString color;

    bool isImage = false;
    bool isTable = false;
    QString imagePath;
    int imageNum;
    double height;
    double width;
}TextInfor;

typedef struct OFDImageInfo
{

}OFDImageInfo;

typedef struct OFDLineInfo
{
    QString lineHeight;
    double lineDistance;
    QVector<TextInfo> textInfoContain;
}OFDLineInfo;


//ҳ����Ϣ�ṹ��
typedef struct OFDPageInfo
{
    QString page_PhysicalBox;             //ҳ����������С
    QString leftMargin;
    QString rightMargin;
    QString topMargin;
    QString bottomMargin;
    QVector<OFDLineInfo> lineInfoContain;
}OFDPageInfo;

class OFDProcess
{
public:
    OFDProcess(QString);

    void initDocInfo();                   //��ʼ���ĵ���Ϣ
    void initPageInfo();                  //��ʼ��ҳ����Ϣ
    void initTextInfo(TextInfo*);                  //��ʼ���ı���Ϣ
    QString findFontID(QString);          //������ID�����ҵ�ID

    bool createOFD(QVector<PageInfo>);                     //����OFD�ĵ�
    bool createOFD_xml();                 //����OFD.xml
    bool createDocument_xml();            //����Document.xml
    bool createPublicRes_xml();           //����PublicRes.xml
    bool createContent_xml(int, OFDPageInfo);             //����Content.xml
    bool createDocumentRes_xml();         //����DocumentRes.xml
    bool createZip();                     //����ZIP��OFD�ĵ�����֯��ʽ

    void setPath();                       //�޸��ĵ�����λ��
    void setFileName();                   //�޸��ĵ���

    void setDocInfo();                    //�޸��ĵ���Ϣ
    void setPageInfo();                   //�޸�ҳ����Ϣ
    void setTextInfo();                   //�޸��ı���Ϣ

    QVector<OFDLineInfo> transformBlockInfo(PageInfo*);
    QString calculateX(FragmentInfo);             //����TextCode XֵQVector<BlockInfo> blockInfoContain
    QString calculateY(FragmentInfo);             //����TextCode Yֵ
    QString calculateDeltaX(TextInfo);            //����TextCode DeltaXֵ
    double calculateBoundary_dx(TextInfo);
    QString calculateBoundary_str(Boundary);          //����TextObject boundaryֵ
    void resetLineHeight(OFDLineInfo*);
    void transformText(QVector<PageInfo>);        //�ӿں������ӽ����ļ��н����ı�����
    QString transformPhysicalBox(PageInfo);
    void copyImage(QString, int);
    static bool DeleteDirectory(QString);

private:
    QXmlStreamWriter writer;              //XMLд����
    QXmlStreamReader reader;              //XML��ȡ��
    QString file_path;                    //�ĵ�·��
    QString file_name;                    //�ĵ�����
    DocInfo doc_info;                     //�ĵ���Ϣ
    QVector<OFDPageInfo> ofdPageInfoContain;
    QVector<DocInfo>  docInfoContain;

    QString fontMap[100] = {"23", "����", "����", "���� Light", "����", "����", "��������", "����Ҧ��",
                           "������", "΢���ź� Light", "΢���ź�", "��Բ", "����", "��������", "�����п�",
                           "����ϸ��", "��������", "���Ŀ���", "������κ", "���Ĳ���", "��������", "���ķ���",
                           "��������","����"};
};

#endif // OFDPROCESS_H
