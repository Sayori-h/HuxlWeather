#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "weathertool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try
        {
            WeatherTool weatherTool; // 在这里创建WeatherTool对象，可能会抛出异常
        }
        catch (const std::exception &e)
        {
            // 处理异常的代码
            qDebug() << "Exception caught: " << e.what();
            // 可以显示错误消息或者执行其他必要的操作
            return 1; // 返回一个非零值表示程序异常退出
        }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "HuxlWeather_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
