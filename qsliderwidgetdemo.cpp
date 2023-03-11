#include "qsliderwidgetdemo.h"
#include "ui_qsliderwidgetdemo.h"

#include "qtsliderwidget.h"
#include <QPushButton>
#include <QDebug>

QSliderWidgetDemo::QSliderWidgetDemo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QSliderWidgetDemo)
{
    ui->setupUi(this);

    setStyleSheet("QWidget#widget, #widget_2{border-image: url(:/image/test_bg.jpeg);}");

    HorizontalSliderTest();
    VerticalSliderTest();

    ui->tabWidget->setCurrentIndex(0);
}

QSliderWidgetDemo::~QSliderWidgetDemo()
{
    delete ui;
}

void QSliderWidgetDemo::HorizontalSliderTest()
{
    m_hWin = new QtSliderWidget(ui->widget, QtSliderWidget::HORIZONTAL_LAYOUT);

    QVBoxLayout *winLayout = new QVBoxLayout;
    winLayout->setMargin(0);
    winLayout->setSpacing(0);
    ui->widget->setLayout(winLayout);
    winLayout->addWidget(m_hWin);
    connect(m_hWin, &QtSliderWidget::SigPageSwitch, this, &QSliderWidgetDemo::HandleHBoxSwitch);

    QStringList color;
    color << "#69c9b1" << "#69b1c9" << "#c9b169" << "#6981c9" << "#c969b1";

    // 横向滑动切换
    for (int i = 0; i < 5; i++ ) {
        QWidget *page = new QWidget();
        page->setObjectName(QString("testHPage%1").arg(i));
        page->setFixedWidth(ui->widget->width());  // !Fixed 必要

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->setSpacing(10);
        page->setLayout(layout);

        for (int j = 0; j < 5; j++ ) {
            QPushButton *btn = new QPushButton;
            btn->setObjectName(QString("HPage%1_TestBtn%2").arg(i + 1).arg(j + 1));
            btn->setText(QString("HPage%1_测试按钮%2").arg(i + 1).arg(j + 1));
            btn->resize(80, 35);
            btn->setStyleSheet(QString("QPushButton{background:%1;}").arg(color.at(i)));
            layout->addWidget(btn);

            connect(btn, &QPushButton::clicked, this, &QSliderWidgetDemo::HandleHBoxBtn);
        }

        m_hWin->AddPages(page);
        qDebug() << "page size : " << page->size();
    }
}

void QSliderWidgetDemo::VerticalSliderTest()
{
    m_vWin = new QtSliderWidget(ui->widget_2, QtSliderWidget::VERTICAL_LAYOUT);
    QHBoxLayout *winLayout = new QHBoxLayout;
    winLayout->setMargin(0);
    winLayout->setSpacing(0);
    ui->widget_2->setLayout(winLayout);
    winLayout->addWidget(m_vWin);
    connect(m_vWin, &QtSliderWidget::SigPageSwitch, this, &QSliderWidgetDemo::HandleVBoxSwitch);

    QStringList color;
    color << "#69c9b1" << "#69b1c9" << "#c9b169" << "#6981c9" << "#c969b1";

    // 纵向滑动切换
    for (int i = 0; i < 5; i++ ) {
        QWidget *page = new QWidget();
        page->setObjectName(QString("testVPage%1").arg(i));
        page->setFixedHeight(ui->widget_2->height()); // !Fixed 必要

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->setSpacing(10);
        page->setLayout(layout);

        for (int j = 0; j < 5; j++ ) {
            QPushButton *btn = new QPushButton;
            btn->setObjectName(QString("VPage%1_TestBtn%2").arg(i + 1).arg(j + 1));
            btn->setText(QString("VPage%1_测试按钮%2").arg(i + 1).arg(j + 1));
            btn->resize(80, 35);
            btn->setStyleSheet(QString("QPushButton{background:%1;}").arg(color.at(i)));
            layout->addWidget(btn);

            connect(btn, &QPushButton::clicked, this, &QSliderWidgetDemo::HandleVBoxBtn);
        }

        m_vWin->AddPages(page);
        qDebug() << "page size : " << page->size();
    }
}

void QSliderWidgetDemo::HandleHBoxBtn()
{
     qDebug() << "HBox clicked button :" << QObject::sender()->objectName();
}

void QSliderWidgetDemo::HandleVBoxBtn()
{
    qDebug() << "VBox clicked button :" << QObject::sender()->objectName();
}

void QSliderWidgetDemo::HandleHBoxSwitch(int index)
{
    qDebug() << "horizontal_layout page switch to :" << index;
}

void QSliderWidgetDemo::HandleVBoxSwitch(int index)
{
    qDebug() << "vertical_layout page switch to :" << index;
}

void QSliderWidgetDemo::on_btnLeft_clicked()
{
    m_hWin->SetCurrentIndex(m_hWin->CurrentIndex() - 1);
}

void QSliderWidgetDemo::on_btnRight_clicked()
{
    m_hWin->SetCurrentIndex(m_hWin->CurrentIndex() + 1);
}

void QSliderWidgetDemo::on_btnUp_clicked()
{
    m_vWin->SetCurrentIndex(m_vWin->CurrentIndex() - 1);
}

void QSliderWidgetDemo::on_btnDown_clicked()
{
    m_vWin->SetCurrentIndex(m_vWin->CurrentIndex() + 1);
}

void QSliderWidgetDemo::on_hBorderCheck_stateChanged(int arg1)
{
    if (arg1 == 0) {
        m_hWin->UpdateQssBorder(0);
    } else {
        m_hWin->UpdateQssBorder(1);
    }
}

void QSliderWidgetDemo::on_vBorderCheck_stateChanged(int arg1)
{
    if (arg1 == 0) {
        m_vWin->UpdateQssBorder(0);
    } else {
        m_vWin->UpdateQssBorder(1);
    }
}
