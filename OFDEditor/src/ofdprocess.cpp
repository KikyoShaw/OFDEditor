#include "ofdprocess.h"

OFDProcess::OFDProcess(QString filename)
{
    initDocInfo();
 //   initPageInfo();
 //   initTextInfo();

    file_name = filename;
    file_path = "./";
}

//��ʼ���ĵ���Ϣ
void OFDProcess::initDocInfo()
{
    doc_info.author = "shaw";
    doc_info.creator = "OFD Editor";
    doc_info.creatorVersion = "1.0";
    doc_info.title = "Unknown";
    doc_info.uuid = QUuid::createUuid();
}

//��ʼ��ҳ����Ϣ
void OFDProcess::initPageInfo()
{

}

//��ʼ���ı���Ϣ
void OFDProcess::initTextInfo(TextInfo* textInfo)
{
    textInfo->size = "6";
    textInfo->boundary.dy = 0.0;
    textInfo->boundary.dx = 0.0;
    textInfo->boundary.x = 0.0;
    textInfo->boundary.y = 0.0;
    textInfo->bold = "400";
    textInfo->italic = "false";
    textInfo->underLine = "false";
    textInfo->color = "0 0 0";
}

//����ID����ʼ��һ��100������һά���飬��һλΪ�������������
QString OFDProcess::findFontID(QString fontFamilay)
{
    for(int i=1; i<=fontMap[0].toInt(); i++)
    {
        if(fontMap[i] == fontFamilay)
            return QString::number(i);    //��intת��ΪQString����
    }

    return QString::number(1);
}

//�ƶ�ͼƬ��Դ
void OFDProcess::copyImage(QString imagePath, int i)
{
    //bug�����ֱ�ӷ�ͼƬ���ƻ�ʧ��,Ҫ���������ֺ���ܸ���ͼƬ
    if(QFile::copy(imagePath, file_path+"Doc_0/Res/image_"+QString::number(i)+"."+imagePath.right(3)))
        qDebug() << "�Ҹ�����һ��ͼƬ" << i;
    else
    {
       qDebug() << "���Ƴ���" << i;
       qDebug() << imagePath;
       qDebug() << file_path+"Doc_0/Res/image_"+QString::number(i)+"."+imagePath.right(3);
    }

}

//����OFD�ĵ�
bool OFDProcess::createOFD(QVector<PageInfo> pageInfoContain)
{
    //ע���ļ���Ŀ¼�Ĵ���˳�򣡣�����
    QDir dir;
    dir.mkdir(file_path+"Doc_0");
    dir.mkdir(file_path+"Doc_0/Res");
    //�ı�ת�������ڴ����ĵ��ܹ�֮ǰ�������ĵ��ṹû�д�����ͼƬ·���޷�ʹ��
    transformText(pageInfoContain);
    createOFD_xml();

    createDocument_xml();
    createPublicRes_xml();
    createDocumentRes_xml();

    //��ʵ���������ҳ���ļ���
    dir.mkdir(file_path+"Doc_0/Pages");
    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        qDebug() << "���ڴ���ҳ��";
        dir.mkdir(file_path+"Doc_0/Pages/Page_"+QString::number(i));
        createContent_xml(i, *it);
        i++;
    }
    //����OFD�ĵ��ṹ
    createZip();

    //ɾ����ʱ�ļ�����ֹ�´γ���
    QFile::remove("OFD.xml");
    DeleteDirectory("Doc_0");
    return true;
}

//����OFD.xml
bool OFDProcess::createOFD_xml()
{

    QFile file(file_path+"OFD.xml");
    QString uuid_QString = doc_info.uuid.toString();                   //ת��ΪQString����
    for(int i=0; i<uuid_QString.length(); i++)    //ȥ��������ַ�
    {
        if(uuid_QString.at(i)=='{'||uuid_QString.at(i)=='}'||uuid_QString.at(i)=='-')
        {
            uuid_QString.remove(i, 1);
            i--;
        }
    }

    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;
    writer.setDevice(&file);                                           //���豸���ص�����
    writer.setAutoFormatting(true);                                    //�Զ�����
    writer.setAutoFormattingIndent(2);                                 //��������

    writer.writeStartDocument();                                       //�ĵ���ʼ
    writer.writeNamespace("http://www.ofdspec.org", "ofd");            //�����ռ�
    writer.writeStartElement("ofd:OFD");                            //OFD��ǩ��ʼ
    writer.writeAttribute("Version", "1.0");
    writer.writeAttribute("DocType", "OFD");

    writer.writeStartElement("ofd:DocBody");                        //DocBody��ǩ��ʼ
    writer.writeStartElement("ofd:DocInfo");                       //DocInfor��ǩ��ʼ

    writer.writeTextElement("ofd:Title", doc_info.title);
    writer.writeTextElement("ofd:DocID", uuid_QString);
    writer.writeTextElement("ofd:Auther", doc_info.author);
    writer.writeTextElement("ofd:Creator", doc_info.creator);
    writer.writeTextElement("ofd:CreatorVersion", doc_info.creatorVersion);

    writer.writeEndElement();                                          //DocInfo��ǩ����
    writer.writeTextElement("ofd:DocRoot", "Doc_0/Document.xml");

    writer.writeEndElement();                                          //DocBody��ǩ��ʼ
    writer.writeEndElement();                                          //OFD��ǩ����
    writer.writeEndDocument();                                         //�ĵ�����

    file.close();

    return true;
}

bool OFDProcess::createDocument_xml()
{
    QFile file(file_path+"Doc_0/Document.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Document");                       //Document��ǩ��ʼ
    writer.writeStartElement("ofd:CommonData");                     //CommonData��ǩ��ʼ

 //   writer.writeTextElement("ofd:MaxUnitID", "200");
    writer.writeStartElement("ofd:PageArea");                       //PageArea��ǩ��ʼ
    writer.writeTextElement("ofd:PhysicalBox", A4page_page_PhysicalBox);       //Ĭ�ϵ���������С��ȡ����ҳ�����������С
    writer.writeEndElement();                                          //PageArea��ǩ����
    writer.writeTextElement("ofd:PublicRes", "PublicRes.xml");
    writer.writeTextElement("ofd:DocumentRes", "DocumentRes.xml");
    writer.writeEndElement();                                          //CommonData��ǩ����

    writer.writeStartElement("ofd:Pages");                          //pages��ǩ��ʼ

    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        writer.writeEmptyElement("ofd:Page");
        writer.writeAttribute("BaseLoc", "Pages/Page_"+QString::number(i)+"/Content.xml");
        writer.writeAttribute("ID", QString::number(100+i));                                  //ID�ţ�ÿһҳΨһ
        i++;
    }

    writer.writeEndElement();                                          //Pages��ǩ����

    writer.writeStartElement("ofd:Action");                         //Action��ǩ��ʼ
    writer.writeEndElement();                                          //Action��ǩ����

    writer.writeStartElement("ofd:Bookmark");                       //BookMark��ǩ��ʼ
    writer.writeEndElement();                                          //BookMark��ǩ����
    writer.writeEndElement();                                          //Document��ǩ����
    writer.writeEndDocument();

    file.close();
    return true;

}


//���������ID��
bool OFDProcess::createPublicRes_xml()
{
    QFile file(file_path+"Doc_0/PublicRes.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Res");                            //Res��ǩ��ʼ
    writer.writeAttribute("BaseLoc", "Res");
    writer.writeStartElement("ofd:Fonts");                          //Fonts��ǩ��ʼ

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "1");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "2");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("���� Light"));
    writer.writeAttribute("ID", "3");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "4");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "5");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��������"));
    writer.writeAttribute("ID", "6");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����Ҧ��"));
    writer.writeAttribute("ID", "7");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("������"));
    writer.writeAttribute("ID", "8");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("΢���ź� Light"));
    writer.writeAttribute("ID", "9");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("΢���ź�"));
    writer.writeAttribute("ID", "10");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��Բ"));
    writer.writeAttribute("ID", "11");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "12");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��������"));
    writer.writeAttribute("ID", "13");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("�����п�"));
    writer.writeAttribute("ID", "14");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����ϸ��"));
    writer.writeAttribute("ID", "15");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��������"));
    writer.writeAttribute("ID", "16");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("���Ŀ���"));
    writer.writeAttribute("ID", "17");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("������κ"));
    writer.writeAttribute("ID", "18");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("���Ĳ���"));
    writer.writeAttribute("ID", "19");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��������"));
    writer.writeAttribute("ID", "20");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("���ķ���"));
    writer.writeAttribute("ID", "21");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("��������"));
    writer.writeAttribute("ID", "22");

    writer.writeEmptyElement("ofd:Font");                           //Font��ǩ��ʼ
    writer.writeAttribute("FontName", QStringLiteral("����"));
    writer.writeAttribute("ID", "23");

    writer.writeEndElement();                                          //Fonts��ǩ����
    writer.writeEndElement();                                          //Res��ǩ����
    writer.writeEndDocument();

    file.close();
    return true;
}

bool OFDProcess::createDocumentRes_xml()
{
    QFile file(file_path+"Doc_0/DocumentRes.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Res");                            //Res��ǩ��ʼ
    writer.writeAttribute("BaseLoc", "Res");
    writer.writeStartElement("ofd:MultiMedias");                    //MultiMedias��ǩ��ʼ

    //ѭ���ҳ����е�ͼƬ��Դ
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        QVector<OFDLineInfo> lineInfoContain = it->lineInfoContain;
        for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
        {
            QVector<TextInfo> textInfoContain = it->textInfoContain;
            for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
            {
                qDebug() << "������ͼƬ";
                if(it->isImage)
                {
                    qDebug() << "�ҵ���";
                    qDebug() << "ID: " << it->imageNum;
                    //ѭ��д��multimadia��mediafile��ǩ
                    writer.writeStartElement("ofd:MultiMedia");                    //MultiMedia��ǩ��ʼ
                    writer.writeAttribute("ID", QString::number(100+it->imageNum));
                    writer.writeAttribute("Type", "Image");
                    writer.writeTextElement("ofd:MediaFile", "image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3));  //ͼƬ����
                    writer.writeEndElement();                                      //MultiMedia��ǩ����
                }
            }
        }
    }

    writer.writeEndElement();                                       //MultiMedias��ǩ����
    writer.writeEndElement();                                       //Res��ǩ����
    writer.writeEndDocument();
    file.close();
    return true;
}


bool OFDProcess::createZip()
{
    QZipWriter zip_writer(file_name);
    qDebug() << file_name;
    //��Ŀ¼��ӵ�ZIP�ļ���
    zip_writer.addDirectory("Doc_0/Pages");
    zip_writer.addDirectory("Doc_0/Res");
    zip_writer.addDirectory("Doc_0/Signs");

    //����ļ�
    QFile file(file_path+"OFD.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("OFD.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/Document.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/Document.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/PublicRes.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/PublicRes.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/DocumentRes.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/DocumentRes.xml", file.readAll());
    file.close();

    //ѭ���ҳ����е�ͼƬ��Դ��д��ofd�ĵ���
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        QVector<OFDLineInfo> lineInfoContain = it->lineInfoContain;
        for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
        {
            QVector<TextInfo> textInfoContain = it->textInfoContain;
            for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
            {
                if(it->isImage)
                {
                    file.setFileName(file_path+"Doc_0/Res/image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3));
                    if(!file.open(QIODevice::ReadOnly)) return false;
                    zip_writer.addFile("Doc_0/Res/image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3), file.readAll());
                    file.close();
                }
            }
        }
    }

    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        file.setFileName(file_path+"Doc_0/Pages/Page_"+QString::number(i)+"/Content.xml");
        if(!file.open(QIODevice::ReadOnly)) return false;
        zip_writer.addFile("Doc_0/Pages/Page_"+QString::number(i)+"/Content.xml", file.readAll());
        file.close();
        i++;
    }

    zip_writer.close();

    return true;
}
//����ɾ���ǿ��ļ���
bool OFDProcess::DeleteDirectory(QString path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            DeleteDirectory(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}

//�������ڱ�������ݲ��뵽ҳ���У����ص�����
bool OFDProcess::createContent_xml(int i, OFDPageInfo ofdPageInfo)
{
    QFile file((file_path+"Doc_0/Pages/Page_%1/Content.xml").arg(QString::number(i)));
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Page");                           //Page��ǩ��ʼ
    writer.writeStartElement("ofd:Area");                           //Area��ǩ��ʼ
    writer.writeStartElement("ofd:PhysicalBox");                    //PhysicalBox
    writer.writeCharacters(ofdPageInfo.page_PhysicalBox);           //д����������С
    writer.writeEndElement();                                       //PhysicalBox
    writer.writeEndElement();                                       //Area��ǩ����
    writer.writeStartElement("ofd:Content");                        //Content��ǩ��ʼ
    writer.writeStartElement("ofd:Layer");                          //Layer��ǩ��ʼ
    writer.writeAttribute("Type", "body");


    //��textInfoContain�����е�����д����ļ�
    QVector<OFDLineInfo> lineInfoContain = ofdPageInfo.lineInfoContain;
    for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
    {
        double lineHeight = it->lineHeight.toDouble();
        double lineDistance = it->lineDistance;
        QVector<TextInfo> textInfoContain = it->textInfoContain;
        for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
        {
            //д���ı�
            if(!it->isImage)
            {
                writer.writeStartElement("ofd:TextObject");                     //TextObject��ǩ��ʼ
        //      writer.writeAttribute("CTM", textInfo.CTM);                    //CTM  ��ʹ��CTMһ�����ԣ�������һ������ߴ���ϰ�����������������
                writer.writeAttribute("Boundary", it->boundary.str);
                qDebug() << "boundary" << it->boundary.str;

                writer.writeAttribute("Font", it->fontID);
                writer.writeAttribute("Size", it->size);
                writer.writeAttribute("Italic", it->italic);                    //б��
                writer.writeAttribute("Weight", it->bold);
                writer.writeEmptyElement("ofd:FillColor");
                writer.writeAttribute("Value", it->color);
                writer.writeStartElement("ofd:TextCode");                       //TextCode��ǩ��ʼ
                writer.writeAttribute("X", it->X);
                writer.writeAttribute("Y", it->Y);
                writer.writeAttribute("DeltaX", it->DeltaX);
                writer.writeCharacters(it->text);
                qDebug() << "д���ı���" << it->text;
                writer.writeEndElement();                                          //TextCode��ǩ����  ������ǩ�϶�Ҫд��ѭ�����氡������
                writer.writeEndElement();                                          //TextObject��ǩ����

                //�����»��ߣ�ͼ�ζ���
                if(it->underLine == "true")
                {
                    writer.writeStartElement("ofd:PathObject");
                    Boundary boundary;
                    boundary.x = it->boundary.x;
                    boundary.y = it->boundary.y+lineHeight;
                    boundary.dx = it->boundary.dx;
                    boundary.dy = 0.05;
                    boundary.str = calculateBoundary_str(boundary);
                    writer.writeAttribute("Boundary", boundary.str);
                    writer.writeTextElement("ofd:AbbreviatedData", "M 0 0.05 L "+QString::number(it->boundary.dx)
                                            +" 0.05 L "+QString::number(it->boundary.dx)+" 0 L 0 0 C");
                    writer.writeEndElement();
                }
                //���Ʊ��
                if(it->isTable)
                {
                    writer.writeStartElement("ofd:PathObject");
                    Boundary boundary;
                    boundary.x = it->boundary.x;
                    boundary.y = it->boundary.y-0.2;   //��ֹͷ������
                    boundary.dx = it->boundary.dx;
                    boundary.dy = lineHeight + lineDistance;
                    boundary.str = calculateBoundary_str(boundary);
                    writer.writeAttribute("Boundary", boundary.str);
                    writer.writeTextElement("ofd:AbbreviatedData", "M 0 0 L "+QString::number(it->boundary.dx)+" 0 L "
                                            +QString::number(it->boundary.dx)+" "+QString::number(lineHeight+lineDistance)
                                            +" L 0 "+QString::number(lineHeight+lineDistance)+" C");
                    writer.writeEndElement();
                }
            }
            //д��ͼƬ
            else
            {
                writer.writeEmptyElement("ofd:ImageObject");                      //ImageObject
                writer.writeAttribute("ResourceID", QString::number(it->imageNum+100));
                writer.writeAttribute("Boundary", it->boundary.str);
                //ͼ������CTM��������
                writer.writeAttribute("CTM", QString::number(CTImage*it->width)+" 0 0 "+QString::number(CTImage*it->height)+" 0 0");
            }

        }
        qDebug() << "����һ�У�����������������";

    }

    writer.writeEndElement();                                          //Layer��ǩ����
    writer.writeEndElement();                                          //Content��ǩ����
    writer.writeEndElement();                                          //Page��ǩ����
    writer.writeEndDocument();

    file.close();
    return true;
}

QString OFDProcess::calculateDeltaX(TextInfo textInfo)
{
    //����DeltaX�������ַ�ƫ�ƾ���Ϊÿ���ַ��ĳ��ȣ���ÿ���ַ������ţ����ı��ּ�࣬����Ե��������������
    QString Deltax;                                  //DeltaxΪ�ַ�֮���ƫ�ƾ��룬�������ַ���-1��
    for(int i=0; i<textInfo.text.length()-1; i++)
    {
        //�ҳ��ַ��������ĸ�Ϳո񣬴�С*CTLetter
        if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)
            Deltax.append(QString::number(textInfo.size.toDouble()*CTLetter*textInfo.dxScale)).append(" ");
        else
            Deltax.append(QString::number(textInfo.size.toDouble()*textInfo.dxScale)).append(" ");
    }
    Deltax.chop(1);       //ȥ�����һ���ո�
    return Deltax;
}

double OFDProcess::calculateBoundary_dx(TextInfo textInfo)
{
    textInfo.boundary.dx = 0.0;
    for(int i=0; i<textInfo.text.length(); i++)   //ת���ı������Ӣ���ַ�
    {
        if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)
            textInfo.boundary.dx = textInfo.boundary.dx+textInfo.size.toDouble()*textInfo.dxScale*CTLetter;
        else
            textInfo.boundary.dx = textInfo.boundary.dx+textInfo.size.toDouble()*textInfo.dxScale;
    }
    return textInfo.boundary.dx;
}

QString OFDProcess::calculateBoundary_str(Boundary boundary)
{
    boundary.str = QString::number(boundary.x).append(" ").append(QString::number(boundary.y)).append(" ")
            .append(QString::number(boundary.dx)).append(" ").append(QString::number(boundary.dy));
    return boundary.str;
}

//��һ�д������������������иߣ�����������boundary.dy��boundary.str
void OFDProcess::resetLineHeight(OFDLineInfo* lineInfo)
{
    lineInfo->lineHeight = QString::number(lineInfo->textInfoContain.begin()->boundary.dy);
    double y = lineInfo->textInfoContain.begin()->boundary.y;        //�����и�
    for(QVector<TextInfo>::iterator it = lineInfo->textInfoContain.begin();it!=lineInfo->textInfoContain.end(); it++)
    {
        if(it->boundary.dy > lineInfo->lineHeight.toDouble())
            lineInfo->lineHeight = QString::number(it->boundary.dy);
    }
    for(QVector<TextInfo>::iterator it = lineInfo->textInfoContain.begin();it!=lineInfo->textInfoContain.end(); it++)
    {

        if(it->isImage)         //ͼƬͨ������boundary.yʹ�·�����
        {
            it->boundary.y = y + lineInfo->lineHeight.toDouble() - it->boundary.dy - BaseLine;    //baseLine��ֹ�Ϸ�����һ��
        }
        else              //����ͨ������Yʹ�ı��·�����
        {
            it->boundary.dy = lineInfo->lineHeight.toDouble();
            it->Y = QString::number(lineInfo->lineHeight.toDouble()- BaseLine);    //ʹ�ı��·����룬 baseLine��ֹ�·�����һ��
        }
        it->boundary.str = calculateBoundary_str(it->boundary);
    }
    qDebug() << "�иߣ�" << lineInfo->lineHeight;
}

//�˺��������������أ��ı��ṹ���ң�������
QVector<OFDLineInfo> OFDProcess::transformBlockInfo(PageInfo *pageInfo)
{
    QVector<BlockInfo> blockInfoContain = pageInfo->blockInfoContain;
    QVector<OFDLineInfo> lineInfoContain;
    OFDLineInfo lineInfo;
    TextInfo textInfo;

    FragmentInfo lastFrag;
    lastFrag.isTable = false;
    initTextInfo(&textInfo);

    textInfo.dxScale = pageInfo->letterDistanceScale;
    textInfo.boundary.y = CTMargin*pageInfo->pageMargin.topMargin;      //���ϸı�
    qDebug() << "��ʼY����"<<  textInfo.boundary.y;
    lineInfo.lineHeight = textInfo.size;
    lineInfo.lineDistance = pageInfo->lineDistance*CTLineDistance;

    for(QVector<BlockInfo>::iterator it = blockInfoContain.begin(); it!=blockInfoContain.end(); it++)
    {
        int lastCell = 0;
        textInfo.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;   //ÿ��block�е�boundary.x��ʼ���赽��ԭ��λ��
        textInfo.boundary.dx = 0.0;      //ÿ��block�е�boundary.dx��ʼ����Ϊ0
        qDebug() << "�ı��ο�ʼY����"<<  textInfo.boundary.y;
        //����ı��εĽ���
        if(it->empty)     //�ı���Ϊ�գ�������У�������textInfo��ֱλ������ƽ��һ��
        {
            qDebug() << "���һ���յ�block";
            textInfo.boundary.y = textInfo.boundary.y+lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;      //ʵ�ֿ���
            qDebug() << "���к�Y����"<<  textInfo.boundary.y;
        }
        else
        {
            //ȡ���ı����е���Ϣ�������ı����ݺ͸�ʽ,��װ��������
            QVector<FragmentInfo> fragmentInfoContain = it->fragmentInfoContain;
            for(QVector<FragmentInfo>::iterator itf = fragmentInfoContain.begin();itf != fragmentInfoContain.end(); itf++)
            {
                lastFrag = *itf;
                qDebug() << lastFrag.isTable;
               //�ж��Ƿ�ͼƬ
               if(!itf->isImage)
               {
                   qDebug() << "���block���ı��Σ�" << itf->text;
                   textInfo.isImage = false;
                   textInfo.isTable = false;
                   textInfo.text = itf->text;
                   textInfo.size = QString::number(CTFont*itf->size);           //����ת��Ϊ�ַ���
                   textInfo.fontID = findFontID(itf->fontFamily);
                   textInfo.Y = textInfo.size;            //YΪ�߽絽�ַ��ĵײ�λ�ã�����Ϊ�ַ��ĸ߶�

                   qDebug() << "�����С" << textInfo.size;

                   textInfo.X = "0";                       //����Ϊ0.��Ӿ��ε�ˮƽ�߽�λ��
                   textInfo.DeltaX = calculateDeltaX(textInfo);
                   textInfo.boundary.x = textInfo.boundary.x+textInfo.boundary.dx;   //֮ǰ��xλ�ü�֮ǰ��ƫ�ƾ���
                   textInfo.boundary.dx = calculateBoundary_dx(textInfo);
                   textInfo.boundary.dy = textInfo.size.toDouble();

                   textInfo.bold = (itf->bold == 50) ? QString::number(400) : QString::number(500);    //�Ӵ�
                   textInfo.italic = itf->italic ? "true" : "false";
                   textInfo.underLine = itf->underLine ? "true" : "false";
                   textInfo.color = itf->color;

                   //���ȼ��  �ı����ȼ����߾����һ���ı��Ŀ�ʼλ��
                   while(textInfo.boundary.dx > pageInfo->pageWidth*CTView - CTMargin*pageInfo->pageMargin.rightMargin-textInfo.boundary.x)
                   {
                       double onebyone = 0.0;
                       for(int i=0; i < textInfo.text.length(); i++)
                       {
                           if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)       //������ĸ�����ֵĿ������
                               onebyone = onebyone+textInfo.size.toDouble()*textInfo.dxScale*CTLetter;
                           else
                               onebyone = onebyone+textInfo.size.toDouble()*textInfo.dxScale;

                           if(onebyone > (pageInfo->pageWidth*CTView-CTMargin*pageInfo->pageMargin.rightMargin-textInfo.boundary.x))
                           {
                               //���ַ������н�ȡ���Ҹ�ʽ��ͬ
                               TextInfo textInfo_left = textInfo;                   //��ȡӦ���ڱ��е��ַ�
                               textInfo_left.text = textInfo.text.left(i);
                               TextInfo textInfo_right = textInfo;
                               textInfo_right.text = textInfo.text.right(textInfo.text.length()-i);   //��ȡӦ�û��е��ַ�

                               textInfo_left.boundary.dx = calculateBoundary_dx(textInfo_left);  //���¼���ƫ�ƾ���
                               textInfo_left.DeltaX = calculateDeltaX(textInfo_left);             //���¼���Deltax

                               lineInfo.textInfoContain.append(textInfo_left);   //���������ּ�����
                               resetLineHeight(&lineInfo);             //�����и�
                               lineInfoContain.append(lineInfo);                 //���ڱ�������մ��һ�У����Խ�lineInfo����������
                               lineInfo.textInfoContain.clear();    //����������
                               qDebug() << "�������������ַ���"<< textInfo_left.text;

                               //����һ��ȫ���ı�,���ﲻ���ǵ����ַ��Ĵ�ֱƫ�ƾ���
                               textInfo_right.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;        //����ص�ԭ�㣻
                               textInfo_right.boundary.dx = calculateBoundary_dx(textInfo_right);           //��Ӿ��ε�ƫ�ƾ���
                               textInfo_right.DeltaX = calculateDeltaX(textInfo_right);                     //���¼���Deltax
                               textInfo_right.boundary.y = textInfo_right.boundary.y + lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;  //��ֱ����ı�
                               textInfo = textInfo_right;       //���½���ѭ����ⳤ��
                               break;  //�˳�ѭ������
                           }
                       }
                       qDebug() << "������";
                   }
                   //�Ա��Ĵ���
                   if(itf->isTable)
                   {
                       textInfo.isTable = true;
                       textInfo.boundary.dx = CTTable*itf->cellWidth;
                       if(itf->cellPosition.row > lastCell)
                       {
                           //��һ�е�Ԫ������
                           textInfo.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;        //����ص�ԭ�㣻
                           textInfo.boundary.y = textInfo.boundary.y + lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;  //��ֱ����ı�
                       }

                       textInfo.bold = (itf->bold == 50) ? QString::number(400) : QString::number(500);    //�Ӵ�
                       textInfo.italic = itf->italic ? "true" : "false";
                       textInfo.underLine = itf->underLine ? "true" : "false";
                       textInfo.color = itf->color;
                       lastCell = itf->cellPosition.row;
                   }                
               }
               //��ͼƬ�Ĵ���
               else
               {
                   qDebug() << "���һ��ͼƬ";
                   //��ͼƬ��Դ�ƶ���OFD��Դ�ļ�����
                   textInfo.isImage = true;
                   textInfo.isTable = false;
                   textInfo.imagePath = itf->imagePath;
                   textInfo.imageNum = itf->imageNum;
                   textInfo.width = itf->width;
                   textInfo.height = itf->height;
                   //ͼƬ��boundary��dx��dy��CTM��a��d������ͬ
                   textInfo.boundary.x = textInfo.boundary.x+textInfo.boundary.dx;   //֮ǰ��xλ�ü�֮ǰ��ƫ�ƾ���
                   textInfo.boundary.dx = CTImage*textInfo.width;
                   textInfo.boundary.dy = CTImage*textInfo.height;
                   calculateBoundary_str(textInfo.boundary);
                   copyImage(textInfo.imagePath, textInfo.imageNum);
               }

               lineInfo.textInfoContain.append(textInfo);
               qDebug() << "����ʱY����"<<  textInfo.boundary.y;
               resetLineHeight(&lineInfo);              //�����и�
               qDebug() << "�������������ַ���"<< textInfo.text;
               qDebug() << "�оࣺ" << lineInfo.lineDistance;
            }
            lineInfoContain.append(lineInfo);
            qDebug() << "һ�н���ǰ��"<<textInfo.boundary.y;
            if(!lastFrag.isTable)
                textInfo.boundary.y = textInfo.boundary.y+lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;         //һ�н�������
            qDebug() << "һ�н������У�"<<textInfo.boundary.y;
            lineInfo.textInfoContain.clear();    //����������
        }
    }
    return lineInfoContain;
}

//����textprocess�е���Ϣ��ȡ������ŵ�ofd��ʽ�ı��ε�������
void OFDProcess::transformText(QVector<PageInfo> pageInfoContain)
{
    //��pageInfoContain�е���Ϣת������ofdPageInfoContain��
    for(QVector<PageInfo>::iterator it = pageInfoContain.begin(); it!=pageInfoContain.end(); it++)
    {
        qDebug() << "�и��Ƿ�ȡ�ã�" << it->lineDistance;
        OFDPageInfo page_info;                   //ҳ����Ϣ
        page_info.leftMargin = QString::number(CTMargin*it->pageMargin.leftMargin);        //����������ת��������
        page_info.rightMargin = QString::number(CTMargin*it->pageMargin.rightMargin);
        page_info.topMargin = QString::number(CTMargin*it->pageMargin.topMargin);
        page_info.bottomMargin = QString::number(CTMargin*it->pageMargin.bottomMargin);
        page_info.page_PhysicalBox = transformPhysicalBox(*it);
        //����pageInfo�е��ı�����Ϣ
        page_info.lineInfoContain = transformBlockInfo(it);
        ofdPageInfoContain.append(page_info);
    }
}

QString OFDProcess::transformPhysicalBox(PageInfo pageInfo)
{
    QString physicalBox = "0 0 ";
    physicalBox.append(QString::number(pageInfo.pageWidth*CTView)).append(" ").append(QString::number(pageInfo.pageHeight*CTView));
    return physicalBox;
}

