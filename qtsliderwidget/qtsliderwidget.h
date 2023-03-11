#ifndef QTSLIDERWIDGET_H
#define QTSLIDERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QTimer>
#include <QLabel>

#define SHOW_BORDER 1

QT_BEGIN_NAMESPACE
namespace Ui { class QtSliderWidget; }
QT_END_NAMESPACE

class QtSliderWidget : public QWidget
{
    Q_OBJECT

    const QSize DOT_SIZE_DEF = QSize(12, 12);
    const QSize DOT_SIZE_SET = QSize(16, 16);
    const int SWITCH_PAGE_ANI_TIME = 240;  // 页面切换动画时间ms
    const int FAST_SLIDER_MOVE_THLD = 30;  // 快速滑动（距离）判断阈值
    const int FAST_SLIDER_TIME_THLD = 300; // 快速滑动（时间ms）判断阈值

public:
    enum LayoutType {
        HORIZONTAL_LAYOUT = 0,
        VERTICAL_LAYOUT,
    };

    QtSliderWidget(QWidget *parent = nullptr, int layoutType = HORIZONTAL_LAYOUT);
    ~QtSliderWidget();

    int AddPages(QWidget *page);
    int PageCount() const;
    int CurrentIndex() const;
    QWidget *CurrentPage() const;
    int PageIndex(QWidget *page) const;

    void UpdateQssBorder(int type); // for demo test

public slots:
    void SetCurrentIndex(int index);
    void SetCurrentPage(QWidget *page);

    void RemovePage(int index);
    void RemovePage(QWidget *page);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

protected slots:
    void HandleScrollerStateChanged(QScroller::State state);
    void OnSliderTimerTimeout();

signals:
    void SigPageSwitch(int index);

private:
    void InitPanel();
    void UpdateDots(int index);
    void HorizontalSliderMove();
    void VerticalSliderMove();

private:
    Ui::QtSliderWidget *ui;
    int m_layoutDirection = 0;
    QWidget *m_navigationWidget;     // 导航栏显示
    QPropertyAnimation *m_switchAni; // 切换动画
    QTimer *m_fastSliderTimer;       // 快滑判断定时器
    bool m_fastSliderFlag = false;   // 快滑标志位: true 快速滑动， false 慢滑动
    QPoint m_startPos;
    QPoint m_endPos;
    int m_curPage = 0;
    int m_nextHPage = -1;
    int m_nextVPage = -1;

};
#endif // QTSLIDERWIDGET_H
