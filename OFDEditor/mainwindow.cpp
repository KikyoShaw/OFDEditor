#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "insertimagedialog.h"
#include "pagesetupdialog.h"
#include "inserttabledialog.h"
#include "setlinedistancedialog.h"
#include "countwordsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{

    //��ʼ��������
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(QStringLiteral("OFD�ı��༭ϵͳ"));

    //��Ӵ������Ĺ�����
    scrollArea = new QScrollArea(this);                      //MainWindow���Ĳ���Ϊ������
    scrollArea->setBackgroundRole(QPalette::Dark);           //��������ɫ
    setCentralWidget(scrollArea);

    //�����Ĺ��������ҳ��
    editor = new RichTextProcess(this);
    editor->resize(editor->getPaperWidth()*editor->getScale(),editor->getPaperHeight()*editor->getPaperNum()*editor->getScale());         //��������editor���ڴ�С��A4������С
    scrollArea->setWidget(editor);                           //������������ı��༭��
    scrollArea->setAlignment(Qt::AlignCenter);               //���ı��༭����ж���

    //�������ѡ�� OFD�Ķ�����֧��Qt��֧�ֵĺܶ����壡����������������
    fontCombo = new QFontComboBox(this);
    ui->toolBar->addWidget(fontCombo);                       //designer��������ѡ�����Ӳ����������У��ô���ʵ��
    fontCombo->setStatusTip(QStringLiteral("��������"));
    connect(fontCombo, SIGNAL(activated(QString)),this,SLOT(textFamily(QString)));      //��������ѡ���Ĳ�

    //����ֺ�ѡ��
    fontSizeCombo = new QComboBox(this);
    ui->toolBar->addWidget(fontSizeCombo);
    fontSizeCombo->setEditable(true);
    fontSizeCombo->setStatusTip(QStringLiteral("�����ֺ�"));
    fontSizeCombo->addItem("11");                            //��༭���ʼ�༭�ֺ����Ӧ
    QFontDatabase db;                                        //�������ѡ��
    foreach(int size, db.standardSizes())
        fontSizeCombo->addItem(QString::number(size));
    connect(fontSizeCombo, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));

    //���״̬��
    pageNum = new QLabel();
    pageNum->setText(QStringLiteral("��1ҳ"));
    statusBar()->addWidget(pageNum);
    connect(editor, SIGNAL(paperNumChanged(int)), this, SLOT(changeStatusBar(int)));
}

//��������ѡ��򣬸ı�����ʱ����Ҫ�ĺ���
void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    editor->setFormat(fmt);
}

//�����ֺ�ѡ��򣬸ı��ֺ�ʱ����Ҫ�ĺ���
void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if(p.toFloat()>0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        editor->setFormat(fmt);
    }
}

void MainWindow::on_action_saveAs_triggered()
{
    editor->saveAs();
}

void MainWindow::on_action_left_triggered()
{
    editor->setAlign(0);
}

void MainWindow::on_action_right_triggered()
{
    editor->setAlign(1);
}

void MainWindow::on_action_center_triggered()
{
    editor->setAlign(2);
}

void MainWindow::on_action_bold_triggered()
{

    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    if(fmt.fontWeight()==QFont::Bold)
        fmt.setFontWeight(QFont::Normal);
    else
        fmt.setFontWeight(QFont::Bold);
    ui->action_bold->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_italic_triggered()
{
    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    fmt.setFontItalic(!fmt.fontItalic());
    ui->action_italic->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_underline_triggered()
{
    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    fmt.setFontUnderline(!fmt.fontUnderline());
    ui->action_underline->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_formatPainter_triggered()
{
   editor->formatPainter(ui->action_formatPainter->isChecked());
}

void MainWindow::on_action_deletePage_triggered()
{
    editor->deleteLastPaper();                //�˴�����֮ǰ�Զ�ɾ��ҳ�����õĲۺ��������С�
    editor->resizePage();   //�˴�ҳ���С��Ҫ����
}

void MainWindow::on_action_newPage_triggered()
{
    editor->createNewPaper();
    editor->resizePage();   //�˴�ҳ���С��Ҫ����
}

void MainWindow::on_action_insertImage_triggered()
{
    InsertImageDialog *dialog = new InsertImageDialog(editor, this);
    dialog->exec();

    //���ɸ��ı�������ʵ�����ͼƬ����
  //  editor->insertImage();
}

void MainWindow::receiveLeftMargin(double num)
{
     editor->getCurrentPaper()->setLeftMargin(num);
}

void MainWindow::receiveRightMargin(double num)
{
     editor->getCurrentPaper()->setRightMargin(num);
}

void MainWindow::receiveTopMargin(double num)
{
     editor->getCurrentPaper()->setTopMargin(num);
}

void MainWindow::receiveBottomMargin(double num)
{
     editor->getCurrentPaper()->setBottomMargin(num);
}

void MainWindow::receiveSetMargainYesButton()
{
    pageMarginDialog->close();
}

void MainWindow::receiveSetMargainResetButton()
{
    receiveLeftMargin(30.00);
    receiveRightMargin(30.00);
    receiveTopMargin(30.00);
    receiveBottomMargin(30.00);
    pageMarginDialog->close();
}

void MainWindow::on_action_pageMargin_triggered()
{
    PageSetupDialog *dialog = new PageSetupDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_color_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black);
    if(!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    editor->setFormat(fmt);
}

void MainWindow::on_action_lineDistance_triggered()
{
    setLineDistanceDialog *dialog = new setLineDistanceDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_undo_triggered()
{
    this->editor->getCurrentPaper()->undo();
}

void MainWindow::on_action_redo_triggered()
{
    this->editor->getCurrentPaper()->redo();
}

void MainWindow::on_action_table_triggered()
{
	//�ҵ�Ŀ�꿪��������
    InsertTableDialog *dialog = new InsertTableDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_saveAsEdit_triggered()
{
	//ˢ�½��棬�ж��Ƿ�������Ŀ��
    editor->saveToRichText();
}

void MainWindow::on_action_open_ofdx_triggered()
{
	//�ٴδ򿪽���
    editor->openRichText();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //�ж��ı��༭���Ƿ��������Ŀ�꣬�����������㣬�����ý���
    if(!editor->editorIsEmpty())
    {
        QMessageBox* msgBox = RichTextProcess::showMessage();
        int ret = msgBox->exec();
        if (ret == QMessageBox::Save)
        {
            editor->saveToRichText();
            editor->resetEditor();
        }
        else if (ret == QMessageBox::Discard)
            editor->resetEditor();
        else
            event->ignore();
        msgBox->deleteLater();
    }
}

void MainWindow::on_action_new_triggered()
{
    if(!editor->editorIsEmpty())
    {
        QMessageBox* msgBox = RichTextProcess::showMessage();
        int ret = msgBox->exec();
        if (ret == QMessageBox::Save)
        {
            editor->saveToRichText();
            editor->resetEditor();
        }
        else if (ret == QMessageBox::Discard)
            editor->resetEditor();
        msgBox->deleteLater();
    }
}

void MainWindow::changeStatusBar(int i)
{
    pageNum->setText("��"+QString::number(i)+"ҳ");
}

void MainWindow::on_action_pageDirection_triggered()
{
    editor->changePaperDirection();
}



void MainWindow::on_action_enlarge_triggered()
{
    editor->enlarge();
}

void MainWindow::on_action_shrink_triggered()
{
  //  editor->shrink();
//    QTextCharFormat charFmt;
//    editor->getCurrentPaper()->selectAll();
  //  editor->getCurrentPaper()->mergeCurrentCharFormat(charFmt);
  //  editor->getCurrentPaper()->mergeCurrentCharFormat(charFmt);

    editor->getCurrentPaper()->zoomIn();
    qDebug() << "zoomIn";
}


/*
// �����¼�
void MainWindow::wheelEvent(QWheelEvent *event)
{

    // ������Զ��ʹ����ʱ���зŴ󣬵�������ʹ���߷�����תʱ������С
    if(event->delta() > 0){
        editor->getCurrentPaper()->zoomIn();
    }else{
        qDebug() << "zoomout";
        editor->getCurrentPaper()->zoomOut();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
//  // �¼�������
//  // �����textEdit�����ϵ��¼�
  if(obj == editor->getCurrentPaper()){
    if(event->type() == QEvent::KeyPress) {
      QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
      if (kevent->key() == Qt::Key_Down) {
          qDebug() << "zoomout";
        editor->getCurrentPaper()->zoomOut();

      }
      else if(kevent->key() == Qt::Key_Up) {
        editor->getCurrentPaper()->zoomIn();
      }
    }
  }
  return QMainWindow::eventFilter(obj,event);
}
*/



void MainWindow::on_action_count_triggered()
{
    CountWordsDialog *dialog = new CountWordsDialog(editor->countWords());
    dialog->exec();
}


void MainWindow::on_action_preview_triggered()
{
    editor->preview();
}
