#ifndef QSLIDERWIDGETDEMO_H
#define QSLIDERWIDGETDEMO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QSliderWidgetDemo; }
QT_END_NAMESPACE

class QtSliderWidget;

class QSliderWidgetDemo : public QMainWindow
{
    Q_OBJECT

public:
    QSliderWidgetDemo(QWidget *parent = nullptr);
    ~QSliderWidgetDemo();

private:
    void HorizontalSliderTest();
    void VerticalSliderTest();

private slots:
    void HandleHBoxBtn();
    void HandleVBoxBtn();
    void HandleHBoxSwitch(int index);
    void HandleVBoxSwitch(int index);

    void on_btnLeft_clicked();
    void on_btnRight_clicked();
    void on_btnUp_clicked();
    void on_btnDown_clicked();
    void on_hBorderCheck_stateChanged(int arg1);
    void on_vBorderCheck_stateChanged(int arg1);

private:
    Ui::QSliderWidgetDemo *ui;
    QtSliderWidget *m_vWin;
    QtSliderWidget *m_hWin;
};
#endif // QSLIDERWIDGETDEMO_H
