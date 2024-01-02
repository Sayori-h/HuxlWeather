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

    forecast_week_list << ui->week0Lb << ui->week1Lb << ui->week2Lb << ui->week3Lb << ui->week4Lb << ui->week5Lb;
    forecast_date_list << ui->date0Lb << ui->date1Lb << ui->date2Lb << ui->date3Lb << ui->date4Lb << ui->date5Lb;
    forecast_aqi_list << ui->quality0Lb << ui->quality1Lb << ui->quality2Lb << ui->quality3Lb << ui->quality4Lb << ui->quality5Lb;
    forecast_type_list << ui->type0Lb << ui->type1Lb << ui->type2Lb << ui->type3Lb << ui->type4Lb << ui->type5Lb;
    forecast_typeIco_list << ui->typeIco0Lb << ui->typeIco1Lb << ui->typeIco2Lb << ui->typeIco3Lb << ui->typeIco4Lb << ui->typeIco5Lb;
    forecast_high_list << ui->high0Lb << ui->high1Lb << ui->high2Lb << ui->high3Lb << ui->high4Lb << ui->high5Lb;
    forecast_low_list << ui->low0Lb << ui->low1Lb << ui->low2Lb << ui->low3Lb << ui->low4Lb << ui->low5Lb;

    //请求天气API信息
    url="http://t.weather.itboy.net/api/weather/city/";
    city=u8"南京";
    cityTmp=city;
    manager=new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::replayFinished);
    getWeatherInfo(manager);

    ui->cityLineEdit->installEventFilter(this);
    ui->curveLb->installEventFilter(this);
    ui->sunRiseSetLb->installEventFilter(this);

    sunTimer=new QTimer(ui->sunRiseSetLb);
    connect(sunTimer,&QTimer::timeout,ui->sunRiseSetLb,QOverload<>::of(&QWidget::update));
    sunTimer->start(1000);


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

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->sunRiseSetLb&&event->type()==QEvent::Paint)
        paintSunRiseSet();
    else if(watched==ui->curveLb&&event->type()==QEvent::Paint)
        paintCurve();
    return QWidget::eventFilter(watched,event);
}

void MainWindow::slotExitApp()
{
    qApp->exit(0);//退出应用程序
}

void MainWindow::replayFinished(QNetworkReply *reply)
{
    // 获取响应的信息，状态码为200表示正常
    QVariant statusCode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // 检查是否可以转换为整数
    bool ok;
    int statusCodeValue = statusCode.toInt(&ok);

    if(reply->error()!=QNetworkReply::NoError||(!ok)||(statusCodeValue!=200)){
        QMessageBox::critical(this,u8"错误",u8"天气：请求数据错误，检查网络连接！");
        return;
    }
    QByteArray bytes=reply->readAll();
    //qDebug()<<bytes;
    parseJson(bytes);
}

void MainWindow::getWeatherInfo(QNetworkAccessManager *manager)
{
    QString cityCode=tool[city];
    if(cityCode=="000000000"){
        QMessageBox::critical(this,u8"错误",u8"天气：指定城市不存在！");
        return;
    }
    QUrl jsonUrl(url+cityCode);
    manager->get(QNetworkRequest(jsonUrl));
}

/* 解析Json数据 */
void MainWindow::parseJson(QByteArray& bytes)
{
    //读取Json数据
    QJsonParseError err;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(bytes,&err);
    if(err.error!=QJsonParseError::NoError)return;

    QJsonObject jsObj=jsonDoc.object();
    //qDebug()<<jsObj;
    //qDebug()<<"==========================================\r\n";
    //qDebug()<<jsObj.value("message").toString();
    QString message=jsObj.value("message").toString();
    if(!message.contains("success")){
        QMessageBox::information(this,u8"来自Json的信息",u8"天气：城市错误！",QMessageBox::Ok);
        city=cityTmp;
        return;
    }
    today=jsObj;
    //qDebug()<<"parseJson:"<<today.ganmao;
    //解析data中的yesterday
    QJsonObject dataObj=jsObj.value("data").toObject();
    forecast[0]=dataObj.value("yesterday").toObject();

    //解析data中的forecast
    QJsonArray forecastArr=dataObj.value("forecast").toArray();
    int j=0;
    for(int i=1;i<6;i++){
        forecast[i]=forecastArr.at(j).toObject();
        j++;
    }
    setLabelContent();
}

void MainWindow::setLabelContent()
{
    // 今日数据
    ui->dateLb->setText(today.date);
    qDebug()<<"Out:"<<today.date;
    ui->temLb->setText(today.wendu);
    qDebug()<<"Out:"<<today.wendu;
    ui->cityLb->setText(today.city);
    ui->typeLb->setText(today.type);
    ui->noticeLb->setText(today.notice);
    ui->shiduLb->setText(today.shidu);
    qDebug()<<"Out:"<<today.shidu;
    ui->pm25Lb->setText(today.pm25);
    ui->fxLb->setText(today.fx);
    ui->flLb->setText(today.fl);
    ui->ganmaoBrowser->setText(today.ganmao);

    // 判断白天还是夜晚图标
    QString sunsetTime = today.date + " " + today.sunset;
    if (QDateTime::currentDateTime() <= QDateTime::fromString(sunsetTime, "yyyy-MM-dd hh:mm"))
    {
        ui->typeIcoLb->setStyleSheet( tr("border-image: url(:/day/day/%1.png); background-color: argb(60, 60, 60, 0);").arg(today.type) );
    }
    else
    {
        ui->typeIcoLb->setStyleSheet( tr("border-image: url(:/night/night/%1.png); background-color: argb(60, 60, 60, 0);").arg(today.type) );
    }

    ui->qualityLb->setText(today.quality == "" ? "无" : today.quality);
    if (today.quality == "优")
    {
        ui->qualityLb->setStyleSheet("color: rgb(0, 255, 0); background-color: argb(255, 255, 255, 0);");
    }
    else if (today.quality == "良")
    {
        ui->qualityLb->setStyleSheet("color: rgb(255, 255, 0); background-color: argb(255, 255, 255, 0);");
    }
    else if (today.quality == "轻度污染")
    {
        ui->qualityLb->setStyleSheet("color: rgb(255, 170, 0); background-color: argb(255, 255, 255, 0);");
    }
    else if (today.quality == "重度污染")
    {
        ui->qualityLb->setStyleSheet("color: rgb(255, 0, 0); background-color: argb(255, 255, 255, 0);");
    }
    else if (today.quality == "严重污染")
    {
        ui->qualityLb->setStyleSheet("color: rgb(170, 0, 0); background-color: argb(255, 255, 255, 0);");
    }
    else
    {
        ui->qualityLb->setStyleSheet("color: rgb(255, 255, 255); background-color: argb(255, 255, 255, 0);");
    }

    //六天数据
    for(int i=0;i<6;i++){
        forecast_week_list.at(i)->setText(forecast[i].week);
        forecast_date_list.at(i)->setText(forecast[i].date);
        forecast_type_list.at(i)->setText(forecast[i].type);
        qDebug()<<"i="<<i<<forecast[i].type;
        forecast_high_list.at(i)->setText(forecast[i].high.split(" ").at(1));
        qDebug()<<"i="<<i<<forecast[i].high.split(" ").at(1);
        forecast_low_list.at(i)->setText(forecast[i].low.split(" ").at(1));
        forecast_typeIco_list.at(i)->setStyleSheet(tr("image: url(:day/day/%1.png);").arg(forecast[i].type));

        if (forecast[i].aqi.toInt() >= 0 && forecast[i].aqi.toInt() <= 50) {
            forecast_aqi_list.at(i)->setText(u8"优质");
            forecast_aqi_list.at(i)->setStyleSheet("color: rgb(0, 255, 0);");
        }
        else if (forecast[i].aqi.toInt() > 50 && forecast[i].aqi.toInt() <= 100){
            forecast_aqi_list.at(i)->setText(u8"良好");
            forecast_aqi_list.at(i)->setStyleSheet("color: rgb(255, 255, 0);");
        }
        else if (forecast[i].aqi.toInt() > 100 && forecast[i].aqi.toInt() <= 150){
            forecast_aqi_list.at(i)->setText(u8"轻度污染");
            forecast_aqi_list.at(i)->setStyleSheet("color: rgb(255, 170, 0);");
        }
        else if (forecast[i].aqi.toInt() > 150 && forecast[i].aqi.toInt() <= 200){
            forecast_aqi_list.at(i)->setText(u8"重度污染");
            forecast_aqi_list.at(i)->setStyleSheet("color: rgb(255, 0, 0);");
        }
        else{
            forecast_aqi_list.at(i)->setText(u8"严重污染");
            forecast_aqi_list.at(i)->setStyleSheet("color: rgb(170, 0, 0);");
        }

    }

    ui->week0Lb->setText(u8"昨天");
    ui->week1Lb->setText(u8"今天");

    ui->curveLb->update();
    return;
}

const QPoint MainWindow::sun[2]={
    QPoint(20,75),QPoint(130,75)
};

const QRect MainWindow::sunRiseSet[3]={
    //x,y,width,height
    QRect(0,80,50,20),//07:20
    QRect(100,80,50,20),//18:01
    QRect(50,80,50,20)//日出日落
};

//绘制虚线圆弧的矩形区域
const QRect MainWindow::arcRect=QRect(25,25,100,100);

void MainWindow::paintSunRiseSet()
{
    QPainter painter(ui->sunRiseSetLb);
    painter.setRenderHint(QPainter::Antialiasing,true);

    //绘制直线
    painter.save();
    QPen pen=painter.pen();
    pen.setWidthF(1.5);
    pen.setColor(Qt::cyan);
    painter.setPen(pen);
    painter.drawLine(sun[0],sun[1]);
    painter.restore();

    // 绘制文字
    painter.save();
    painter.setFont(QFont("阿里巴巴普惠体 M",8,QFont::Normal));
    painter.setPen(Qt::white);

    // 有日出日落时间才绘制
    if(today.sunrise!=""&&today.sunset!=""){
        // 在指定区域绘制文字
        painter.drawText(sunRiseSet[0],Qt::AlignHCenter,today.sunrise);
        painter.drawText(sunRiseSet[1],Qt::AlignHCenter,today.sunset);
    }
    painter.drawText(sunRiseSet[2],Qt::AlignHCenter,u8"日出日落");
    painter.restore();

    // 绘制圆弧
    painter.save();
    pen.setWidthF(1.5);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    painter.drawArc(arcRect,0,180*16);
    painter.restore();

    // 绘制日出日落占比
    if(today.sunrise!=""&&today.sunset!=""){
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(170,255,127,100));

        int stAngle,spanAngle;// 起始角度，旋转角度
        QString sunsetTime=today.date+" "+today.sunset;
        if(QDateTime::currentDateTime()
                >QDateTime::fromString(sunsetTime,"yyyy-MM-dd hh:mm"))
        {
            stAngle=0;
            spanAngle=180*16;
        }
        else{
            // 计算起始角度和跨越角度
            static QStringList sunSetTime=today.sunset.split(":");
            static QStringList sunRiseTime=today.sunrise.split(":");

            static QString sunSetHour=sunSetTime[0];
            static QString sunSetMin=sunSetTime[1];
            static QString sunRiseHour=sunRiseTime[0];
            static QString sunRiseMin=sunRiseTime[1];

            static int sun_rise=sunRiseHour.toInt()*60+sunRiseMin.toInt();
            static int sun_set=sunSetHour.toInt()*60+sunSetMin.toInt();
            static int sun_time=sun_set-sun_rise;
            int now=QTime::currentTime().hour()*60+QTime::currentTime().minute();

            stAngle=((double)(sun_set-now)/sun_time)*180*16;
            spanAngle=((double)(now-sun_rise)/sun_time)*180*16;
        }
        if(stAngle>=0&&spanAngle>=0)
            painter.drawPie(arcRect,stAngle,spanAngle);//扇形绘制
    }
}

void MainWindow::paintCurve()
{
    QPainter painter(ui->curveLb);
    painter.setRenderHint(QPainter::Antialiasing,true);

    // 将温度转换为int类型，并计算平均值，平均值作为curveLb曲线的参考值，参考Y坐标为40
    int tempTotal=0;
    int high[6]{},low[6]{};

    QString h,l;
    for(int i=0;i<6;i++){
        h=forecast[i].high.split(" ").at(1);
        h=h.left(h.length()-1);
        high[i]=(int)(h.toDouble());
        tempTotal+=high[i];

        l=forecast[i].low.split(" ").at(1);
        l=l.left(l.length()-1);
        low[i]=(int)(l.toDouble());
    }
    int tempAverage=(int)(tempTotal/6);// 最高温平均值

    // 算出温度对应坐标
    int pX[6]{35,103,172,241,310,379};// 点X的坐标
    int pHY[6]{},pLY[6]{};
    //求出高低温点的坐标
    for(int i=0;i<6;i++){
        pHY[i]=TEMPERATURE_STARTING_COORDINATE-((high[i]-tempAverage)*SPAN_INDEX);
        pLY[i]=TEMPERATURE_STARTING_COORDINATE+((tempAverage-low[i])*SPAN_INDEX);
    }

    QPen pen=painter.pen();
    pen.setWidthF(1.5);

    // 高温曲线绘制
    //绘制昨天-今天虚线部分
    painter.save();
    pen.setColor(QColor(255,170,0));
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    //画温度点
    painter.setBrush(QColor(255, 170, 0));
    painter.drawEllipse(QPoint(pX[0],pHY[0]),ORIGIN_SIZE,ORIGIN_SIZE);
    painter.drawEllipse(QPoint(pX[1],pHY[1]),ORIGIN_SIZE,ORIGIN_SIZE);
    //连线
    painter.drawLine(pX[0],pHY[0],pX[1],pHY[1]);

    //绘制今天之后的实线部分
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    for(int i=1;i<5;i++){
        painter.drawEllipse(QPoint(pX[i+1],pHY[i+1]),ORIGIN_SIZE,ORIGIN_SIZE);
        painter.drawLine(pX[i],pHY[i],pX[i+1],pHY[i+1]);
    }
    painter.restore();

    // 低温曲线绘制
    //绘制昨天-今天虚线部分
    pen.setColor(QColor(0,255,255));
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    //画温度点
    painter.setBrush(QColor(0,255,255));
    painter.drawEllipse(QPoint(pX[0],pLY[0]),ORIGIN_SIZE,ORIGIN_SIZE);
    painter.drawEllipse(QPoint(pX[1],pLY[1]),ORIGIN_SIZE,ORIGIN_SIZE);
    //连线
    painter.drawLine(pX[0],pLY[0],pX[1],pLY[1]);

    //绘制今天之后的实线部分
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    for(int i=1;i<5;i++){
        painter.drawEllipse(QPoint(pX[i+1],pLY[i+1]),ORIGIN_SIZE,ORIGIN_SIZE);
        painter.drawLine(pX[i],pLY[i],pX[i+1],pLY[i+1]);
    }
}


void MainWindow::on_refreshBt_clicked()
{
    getWeatherInfo(manager);
    ui->curveLb->update();
}


void MainWindow::on_searchBt_clicked()
{
    cityTmp=city;
    city=ui->cityLineEdit->text();
    getWeatherInfo(manager);
}

