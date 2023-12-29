#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口属性为无边框，没有标题栏，菜单栏需在UI中手动删除
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置窗口固定大小
    setFixedSize(width(),height());

    m_pExitMenu=new QMenu(this);
    m_pExitAct=new QAction(this);
    m_pExitAct->setText(u8"退出系统");
    m_pExitAct->setIcon(QIcon(":/weatherIco/close.ico"));
    m_pExitMenu->addAction(m_pExitAct);
    connect(m_pExitAct,&QAction::triggered,this,&MainWindow::slotExitApp);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_pExitMenu->exec(QCursor::pos());// 弹出右键菜单
    event->accept();// 将右键菜单事件标记为已接受
}

// 窗口移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_Pos=event->globalPos()-this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPos()-m_Pos);
}

void MainWindow::slotExitApp()
{
    qApp->exit(0);//退出应用程序
}

