#include "qtsliderwidget.h"
#include "ui_qtsliderwidget.h"

#include <QScroller>
#include <QPushButton>
#include <QDebug>
#include <QFile>

QtSliderWidget::QtSliderWidget(QWidget *parent, int layoutType)
    : QWidget(parent)
    , ui(new Ui::QtSliderWidget)
{
    ui->setupUi(this);

    m_layoutDirection = layoutType;
    InitPanel();

    QFile file(":/res/qss/style.qss");
    if (file.open(QIODevice::ReadOnly)) {
        setStyleSheet(file.readAll());
        ui->widgetContents->setStyleSheet(file.readAll());
        file.close();
    }
}

QtSliderWidget::~QtSliderWidget()
{
    delete ui;
}

void QtSliderWidget::InitPanel()
{
    ui->scrollArea->setAlignment(Qt::AlignCenter);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏滑条
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setProperty("show_border", false);  // 默认不显示边框

    auto scroller = QScroller::scroller(ui->scrollArea);
    scroller->grabGesture(ui->scrollArea, QScroller::LeftMouseButtonGesture);

    auto properties = scroller->scrollerProperties();
    properties.setScrollMetric(QScrollerProperties::SnapTime, 0.8);
    properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 1);
    scroller->setScrollerProperties(properties);

    if (m_layoutDirection == VERTICAL_LAYOUT) {
        QVBoxLayout *cntVLayout = new QVBoxLayout;
        cntVLayout->setMargin(0);
        cntVLayout->setSpacing(0);
        ui->widgetContents->setLayout(cntVLayout);
    } else {
        QHBoxLayout *cntHLayout = new QHBoxLayout;
        cntHLayout->setMargin(0);
        cntHLayout->setSpacing(0);
        ui->widgetContents->setLayout(cntHLayout);
    }

    m_navigationWidget = new QWidget();
    m_navigationWidget->setObjectName("navigationWidget");
    m_navigationWidget->setProperty("show_border", false);

    if (m_layoutDirection == VERTICAL_LAYOUT) {
        QVBoxLayout *navVLayout = new QVBoxLayout;
        navVLayout->setAlignment(Qt::AlignCenter);
        navVLayout->setContentsMargins(6, 0, 6, 0);
        m_navigationWidget->setLayout(navVLayout);
        ui->gridLayout->addWidget(m_navigationWidget, 0, 1);
    } else {
        QHBoxLayout *navHLayout = new QHBoxLayout;
        navHLayout->setAlignment(Qt::AlignCenter);
        navHLayout->setContentsMargins(0, 6, 0, 6);
        m_navigationWidget->setLayout(navHLayout);
        ui->gridLayout->addWidget(m_navigationWidget, 1, 0);
    }

    if (m_layoutDirection == VERTICAL_LAYOUT) {
        m_switchAni = new QPropertyAnimation(ui->scrollArea->verticalScrollBar(), "value", this);
    } else {
        m_switchAni = new QPropertyAnimation(ui->scrollArea->horizontalScrollBar(), "value", this);
    }

    /* QT曲线动画介绍: https://doc.qt.io/qt-5/qml-qtquick-propertyanimation.html */
    m_switchAni->setDuration(SWITCH_PAGE_ANI_TIME);
    m_switchAni->setEasingCurve(QEasingCurve::OutCirc);

    m_fastSliderTimer = new QTimer(this);
    m_fastSliderTimer->setInterval(FAST_SLIDER_TIME_THLD);

    connect(QScroller::scroller(ui->scrollArea), &QScroller::stateChanged, this, &QtSliderWidget::HandleScrollerStateChanged);
    connect(m_fastSliderTimer, &QTimer::timeout, this, &QtSliderWidget::OnSliderTimerTimeout);
}

void QtSliderWidget::UpdateDots(int index)
{
    const QObjectList &dotList = m_navigationWidget->children();
    for (int i = 1; i < dotList.count(); i++) {
        auto dot = qobject_cast<QLabel *>(dotList[i]);
        if (i == index + 1) {
            dot->setPixmap(QPixmap(":/res/image/selectDot.png").scaled(DOT_SIZE_SET, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            dot->setPixmap(QPixmap(":/res/image/defDot.png").scaled(DOT_SIZE_DEF, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

int QtSliderWidget::AddPages(QWidget *page)
{
    ui->widgetContents->layout()->addWidget(page);

    auto dotLabel = new QLabel;
    dotLabel->resize(DOT_SIZE_SET);
    dotLabel->setAlignment(Qt::AlignCenter);
    m_navigationWidget->layout()->addWidget(dotLabel);

    // 初始化默认选择首页
    if (PageCount() == 1) {
        dotLabel->setPixmap(QPixmap(":/res/image/selectDot.png").scaled(DOT_SIZE_SET, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        dotLabel->setPixmap(QPixmap(":/res/image/defDot.png").scaled(DOT_SIZE_DEF, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    return PageCount() - 1;
}

int QtSliderWidget::PageCount() const
{
    return ui->widgetContents->layout()->count();
}

int QtSliderWidget::CurrentIndex() const
{
    return m_curPage;
}

QWidget *QtSliderWidget::CurrentPage() const
{
    if (PageCount() == 0)
        return nullptr;

    return qobject_cast<QWidget*>(ui->widgetContents->children()[m_curPage+1]);
}

int QtSliderWidget::PageIndex(QWidget *page) const
{
    auto index = ui->widgetContents->children().indexOf(page);

    return (index == -1) ? -1 : (index - 1);
}

void QtSliderWidget::UpdateQssBorder(int type)
{
    if (type == 0) {
        ui->scrollArea->setProperty("show_border", false);
        m_navigationWidget->setProperty("show_border", false);

        QList<QWidget *> widgets = ui->widgetContents->findChildren<QWidget *> ();
        for (int index = 0; index < widgets.count(); index++) {
            if (!widgets.at(index)->inherits("QPushButton")) {
                widgets.at(index)->setStyleSheet(QString("QWidget#%1{background: transparent; border: none;}").arg(widgets.at(index)->objectName()));
            }
        }
    } else {
        ui->scrollArea->setProperty("show_border", true);
        m_navigationWidget->setProperty("show_border", true);
        // 内部page页边框设置
        QList<QWidget *> widgets = ui->widgetContents->findChildren<QWidget *> ();
        for (int index = 0; index < widgets.count(); index++) {
            if (!widgets.at(index)->inherits("QPushButton")) {
                widgets.at(index)->setStyleSheet(QString("QWidget#%1{background: transparent; border: 1px solid red;}").arg(widgets.at(index)->objectName()));
            }
        }
    }

    setStyle(QApplication::style());
}

void QtSliderWidget::SetCurrentIndex(int index)
{
    if (index < 0 || index >= PageCount())
        return;

    if (m_layoutDirection == VERTICAL_LAYOUT) {
        m_switchAni->setStartValue(ui->scrollArea->verticalScrollBar()->value());
        m_switchAni->setEndValue(index * this->height());
        m_switchAni->start();
    } else {
        m_switchAni->setStartValue(ui->scrollArea->horizontalScrollBar()->value());
        m_switchAni->setEndValue(index * this->width());
        m_switchAni->start();
    }

    m_curPage = index;
    UpdateDots(m_curPage);
    emit SigPageSwitch(m_curPage);
}

void QtSliderWidget::SetCurrentPage(QWidget *page)
{
    auto index = ui->widgetContents->children().indexOf(page);

    if (index == -1)
        return;

    SetCurrentIndex(index - 1);
}

void QtSliderWidget::RemovePage(int index)
{
    if (index < 0 || index >= PageCount() || PageCount() <= 1)
        return;

    auto widget = qobject_cast<QWidget*>(ui->widgetContents->children()[index+1]);
    auto indicator = qobject_cast<QWidget*>(m_navigationWidget->children()[index+1]);

    ui->widgetContents->layout()->removeWidget(widget);
    m_navigationWidget->layout()->removeWidget(indicator);

    delete widget;
    delete indicator;

    SetCurrentIndex(0);
}

void QtSliderWidget::RemovePage(QWidget *page)
{
    RemovePage(PageIndex(page));
}

void QtSliderWidget::resizeEvent(QResizeEvent *event)
{
    ui->scrollArea->resize(this->size());

    if (m_layoutDirection == VERTICAL_LAYOUT) {
        ui->widgetContents->resize(this->width() - 20, this->height() * ui->widgetContents->layout()->count());
        m_navigationWidget->resize(20, this->height());
    } else {
        ui->widgetContents->resize(this->width() * ui->widgetContents->layout()->count(), this->height() - 20);
        m_navigationWidget->resize(this->width(), 20);
    }

    QWidget::resizeEvent(event);
}

void QtSliderWidget::HorizontalSliderMove()
{
    if (!m_fastSliderFlag) {
        // 慢滑情景，判断滑动是否过半
        int value = ui->scrollArea->horizontalScrollBar()->value();
        m_nextHPage = (value / this->width()) + (value % this->width() / (this->width() / 2));
    } else {
        // 快滑，判断滑动方向
        if (m_startPos.x() - m_endPos.x() > FAST_SLIDER_MOVE_THLD) // 左滑,往后翻页
            m_nextHPage += 1;
        else if (m_endPos.x() - m_startPos.x() > FAST_SLIDER_MOVE_THLD) // 右滑,往前翻页
            m_nextHPage -= 1;

        if (m_nextHPage > PageCount()) // 显示尾页
            m_nextHPage = PageCount() - 1;

        if (m_nextHPage < 0)  // 显示首页
            m_nextHPage = 0;
    }

    // 动效
    m_switchAni->setStartValue(ui->scrollArea->horizontalScrollBar()->value());
    m_switchAni->setEndValue(m_nextHPage * this->width());
    m_switchAni->start();

    if (m_nextHPage != m_curPage) {
        m_curPage = m_nextHPage;
        UpdateDots(m_curPage);
        emit SigPageSwitch(m_curPage);
    }
}

void QtSliderWidget::VerticalSliderMove()
{
    if (!m_fastSliderFlag) {
        // 慢滑情景，判断滑动是否过半
        int value = ui->scrollArea->verticalScrollBar()->value();
        m_nextVPage = (value / this->height()) + (value % this->height() / (this->height() / 2));
    } else {
        // 快滑，判断滑动方向
        if (m_startPos.y() - m_endPos.y() > FAST_SLIDER_MOVE_THLD) // 上滑
            m_nextVPage += 1;
        else if (m_endPos.y() - m_startPos.y() > FAST_SLIDER_MOVE_THLD) // 下滑
            m_nextVPage -= 1;

        if (m_nextVPage > PageCount()) // 显示尾页
            m_nextVPage = PageCount() - 1;

        if (m_nextVPage < 0)  // 显示首页
            m_nextVPage = 0;
    }

    // 动效
    m_switchAni->setStartValue(ui->scrollArea->verticalScrollBar()->value());
    m_switchAni->setEndValue(m_nextVPage * this->height());
    m_switchAni->start();

    if (m_nextVPage != m_curPage) {
        m_curPage = m_nextVPage;
        UpdateDots(m_curPage);
        emit SigPageSwitch(m_curPage);
    }
}

void QtSliderWidget::HandleScrollerStateChanged(QScroller::State state)
{
    if (PageCount() == 0)
        return;

    if (state == QScroller::Inactive) { // 释放 or 松开
        m_fastSliderTimer->stop();
        m_endPos = QCursor::pos();

        if (m_endPos == m_startPos) {
            return;
        }

        if (m_layoutDirection == VERTICAL_LAYOUT) {
            VerticalSliderMove();
        } else {
            HorizontalSliderMove();
        }

        m_fastSliderFlag = false;
        m_startPos = QPoint(0, 0);
        m_endPos = QPoint(0, 0);
    }

    if (state == QScroller::Pressed) { // 点击 or 按下
        if (m_layoutDirection == VERTICAL_LAYOUT) {
            m_nextVPage = m_curPage = ui->scrollArea->verticalScrollBar()->value() / this->height();
        } else {
            m_nextHPage = m_curPage = ui->scrollArea->horizontalScrollBar()->value() / this->width();
        }
        m_startPos = QCursor::pos();
        m_fastSliderTimer->start();
        m_fastSliderFlag = true;
    }
}

void QtSliderWidget::OnSliderTimerTimeout()
{
    m_fastSliderTimer->stop();
    m_fastSliderFlag = false;
}
