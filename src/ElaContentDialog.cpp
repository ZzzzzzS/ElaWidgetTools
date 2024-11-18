#include "ElaContentDialog.h"

#include <ElaPushButton.h>

#include <QApplication>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

#include "ElaMaskWidget.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaWinShadowHelper.h"
#include "private/ElaContentDialogPrivate.h"

ElaContentDialog::ElaContentDialog(QWidget* parent, const QString& titleText, const QString& subtitleText)
    : QDialog{parent}, d_ptr(new ElaContentDialogPrivate())
{
    Q_D(ElaContentDialog);
    d->q_ptr = this;
    
	QWidget* parentWidget = parent;
    while (parentWidget->parentWidget())
    {
		parentWidget = parentWidget->parentWidget();
    }
    this->RootWidget = parentWidget;
    d->_maskWidget = new ElaMaskWidget(parentWidget);
    d->_maskWidget->move(0, 0);
	qDebug() << parentWidget->size();
    d->_maskWidget->setFixedSize(parentWidget->size());
    d->_maskWidget->setVisible(false);

    resize(400, height());
    setWindowModality(Qt::ApplicationModal);
#ifdef Q_OS_WIN
    createWinId();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    window()->setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
#endif
#else
    window()->setWindowFlags((window()->windowFlags()) | Qt::FramelessWindowHint);
#endif
    d->_leftButton = new ElaPushButton("cancel", this);
    connect(d->_leftButton, &ElaPushButton::clicked, this, [=]() {
        Q_EMIT leftButtonClicked();
        onLeftButtonClicked();
        d->_maskWidget->doMaskAnimation(0);
        d->_doCloseAnimation();
    });
    d->_leftButton->setMinimumSize(120, 0);
    d->_leftButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    d->_leftButton->setFixedHeight(38);
    d->_leftButton->setFixedWidth(120);
    //d->_leftButton->setBorderRadius(6);
    d->_middleButton = new ElaPushButton("minimum", this);
    connect(d->_middleButton, &ElaPushButton::clicked, this, [=]() {
        Q_EMIT middleButtonClicked();
        onMiddleButtonClicked();
        d->_doCloseAnimation();
    });
    d->_middleButton->setMinimumSize(120, 0);
    d->_middleButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    d->_middleButton->setFixedHeight(38);
    d->_middleButton->setFixedWidth(120);
    //d->_middleButton->setBorderRadius(6);
    d->_rightButton = new ElaPushButton("exit", this);
    connect(d->_rightButton, &ElaPushButton::clicked, this, [=]() {
        Q_EMIT rightButtonClicked();
        onRightButtonClicked();
        d->_doCloseAnimation();
    });
    d->_rightButton->setLightDefaultColor(ElaThemeColor(ElaThemeType::Light, PrimaryNormal));
    d->_rightButton->setLightHoverColor(ElaThemeColor(ElaThemeType::Light, PrimaryHover));
    d->_rightButton->setLightPressColor(ElaThemeColor(ElaThemeType::Light, PrimaryPress));
    d->_rightButton->setLightTextColor(Qt::white);
    d->_rightButton->setDarkDefaultColor(ElaThemeColor(ElaThemeType::Dark, PrimaryNormal));
    d->_rightButton->setDarkHoverColor(ElaThemeColor(ElaThemeType::Dark, PrimaryHover));
    d->_rightButton->setDarkPressColor(ElaThemeColor(ElaThemeType::Dark, PrimaryPress));
    d->_rightButton->setDarkTextColor(Qt::white);
    d->_rightButton->setMinimumSize(120, 0);
    d->_rightButton->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    d->_rightButton->setFixedHeight(38);
    d->_rightButton->setFixedWidth(120);
    //d->_rightButton->setBorderRadius(6);

    d->_centralWidget = new QWidget(this);
    QVBoxLayout* centralVLayout = new QVBoxLayout(d->_centralWidget);
    centralVLayout->setContentsMargins(15, 25, 15, 10);
    ElaText* title = new ElaText(titleText, this);
    title->setTextStyle(ElaTextType::Title);
    title->adjustSize();
    title->setMinimumHeight(20);
    title->setAlignment(Qt::AlignTop|Qt::AlignLeft);

    ElaText* subTitle = new ElaText(subtitleText, this);
    subTitle->setTextStyle(ElaTextType::Body);
    subTitle->adjustSize();
    subTitle->setMinimumHeight(60);
    centralVLayout->addWidget(title);
    centralVLayout->addSpacing(2);
    centralVLayout->addWidget(subTitle);
    centralVLayout->addStretch();
    this->Title = title;
    this->SubTitle = subTitle;
    this->CentralWidgetChanged = false;

    d->_mainLayout = new QVBoxLayout(this);
    d->_mainLayout->setContentsMargins(0, 0, 0, 0);
    d->_buttonWidget = new QWidget(this);
    d->_buttonWidget->setFixedHeight(60);
    QHBoxLayout* buttonLayout = new QHBoxLayout(d->_buttonWidget);
    QSpacerItem* buttonSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(buttonSpacer);
    buttonLayout->addWidget(d->_leftButton);
    buttonLayout->addWidget(d->_middleButton);
    buttonLayout->addWidget(d->_rightButton);
    d->_mainLayout->addWidget(d->_centralWidget);
    d->_mainLayout->addWidget(d->_buttonWidget);

    d->_themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) { d->_themeMode = themeMode; });
}

ElaContentDialog::~ElaContentDialog()
{
    Q_D(ElaContentDialog);
    d->_maskWidget->deleteLater();
    qDebug() << "good bye ela content";
}

void ElaContentDialog::onLeftButtonClicked()
{
    this->setResult(QDialog::DialogCode::Rejected);
}

void ElaContentDialog::onMiddleButtonClicked()
{
    this->setResult(QDialog::DialogCode::Rejected);
}

void ElaContentDialog::onRightButtonClicked()
{
    this->setResult(QDialog::DialogCode::Accepted);
}

void ElaContentDialog::setTitleSubTitle(const QString& TitleText, const QString SubTitleText)
{
    if (this->CentralWidgetChanged)
        return;

    this->Title->setText(TitleText);
    this->SubTitle->setText(SubTitleText);
    this->Title->adjustSize();
    this->SubTitle->adjustSize();
}
void ElaContentDialog::setCentralWidget(QWidget* centralWidget)
{
    Q_D(ElaContentDialog);
    d->_mainLayout->takeAt(0);
    d->_mainLayout->takeAt(0);
    delete d->_centralWidget;
    this->CentralWidgetChanged = true;
    d->_mainLayout->addWidget(centralWidget);
    d->_mainLayout->addWidget(d->_buttonWidget);
}

void ElaContentDialog::setButtonNumber(size_t number)
{
    Q_D(ElaContentDialog);
    d->q_ptr = this;
    switch (number)
    {
    case 1:
        d->_leftButton->hide();
        d->_middleButton->hide();
        d->_rightButton->show();
        break;
    case 2:
        d->_leftButton->hide();
        d->_middleButton->show();
        d->_rightButton->show();
        break;
    case 3:
        d->_leftButton->show();
        d->_middleButton->show();
        d->_rightButton->show();
        break;
    default:
        break;
    }
}

void ElaContentDialog::setLeftButtonText(QString text, bool heightlight)
{
    Q_D(ElaContentDialog);
    d->_leftButton->setText(text);
    this->SetButtonHeightLight(d->_leftButton, heightlight);
}

void ElaContentDialog::setMiddleButtonText(QString text, bool heightlight)
{
    Q_D(ElaContentDialog);
    d->_middleButton->setText(text);
    this->SetButtonHeightLight(d->_middleButton, heightlight);
}

void ElaContentDialog::setRightButtonText(QString text, bool heightlight)
{
    Q_D(ElaContentDialog);
    d->_rightButton->setText(text);
    this->SetButtonHeightLight(d->_rightButton, heightlight);
}

void ElaContentDialog::SetButtonHeightLight(ElaPushButton* button, bool heightlight)
{
    if (heightlight)
    {
        button->setLightDefaultColor(ElaThemeColor(ElaThemeType::Light, PrimaryNormal));
        button->setLightHoverColor(ElaThemeColor(ElaThemeType::Light, PrimaryHover));
        button->setLightPressColor(ElaThemeColor(ElaThemeType::Light, PrimaryPress));
        button->setLightTextColor(Qt::white);
        button->setDarkDefaultColor(ElaThemeColor(ElaThemeType::Dark, PrimaryNormal));
        button->setDarkHoverColor(ElaThemeColor(ElaThemeType::Dark, PrimaryHover));
        button->setDarkPressColor(ElaThemeColor(ElaThemeType::Dark, PrimaryPress));
        button->setDarkTextColor(Qt::white);
    }
    else
    {
        button->setLightDefaultColor(ElaThemeColor(ElaThemeType::Light, BasicBase));
        button->setDarkDefaultColor(ElaThemeColor(ElaThemeType::Dark, BasicBase));
        button->setLightHoverColor(ElaThemeColor(ElaThemeType::Light, BasicHover));
        button->setDarkHoverColor(ElaThemeColor(ElaThemeType::Dark, BasicHover));
        button->setLightPressColor(ElaThemeColor(ElaThemeType::Light, BasicPress));
        button->setDarkPressColor(ElaThemeColor(ElaThemeType::Dark, BasicPress));
        button->setLightTextColor(ElaThemeColor(ElaThemeType::Light, BasicText));
        button->setDarkTextColor(ElaThemeColor(ElaThemeType::Dark, BasicText));
    }
}

void ElaContentDialog::showEvent(QShowEvent* event)
{
    Q_D(ElaContentDialog);
    d->_maskWidget->setVisible(true);
    d->_maskWidget->raise();
    d->_maskWidget->setFixedSize(this->RootWidget->size());
    d->_maskWidget->doMaskAnimation(90);
#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
    HWND hwnd = (HWND)d->_currentWinID;
    setShadow(hwnd);
    DWORD style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    bool hasCaption = (style & WS_CAPTION) == WS_CAPTION;
    if (!hasCaption)
    {
        ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_CAPTION);
    }
#endif
#endif
    QDialog::showEvent(event);
}

void ElaContentDialog::paintEvent(QPaintEvent* event)
{
    Q_D(ElaContentDialog);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ElaThemeColor(d->_themeMode, DialogBase));
    // 背景绘制
    painter.drawRect(rect());
    // 按钮栏背景绘制
    painter.setBrush(ElaThemeColor(d->_themeMode, DialogLayoutArea));
    painter.drawRoundedRect(QRectF(0, height() - 60, width(), 60), 8, 8);
    painter.restore();
}

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool ElaContentDialog::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#else
bool ElaContentDialog::nativeEvent(const QByteArray& eventType, void* message, long* result)
#endif
{
    Q_D(ElaContentDialog);
    if ((eventType != "windows_generic_MSG") || !message)
    {
        return false;
    }
    const auto msg = static_cast<const MSG*>(message);
    const HWND hwnd = msg->hwnd;
    if (!hwnd || !msg)
    {
        return false;
    }
    d->_currentWinID = (qint64)hwnd;
    const UINT uMsg = msg->message;
    const WPARAM wParam = msg->wParam;
    const LPARAM lParam = msg->lParam;
    switch (uMsg)
    {
    case WM_WINDOWPOSCHANGING:
    {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (wp != nullptr && (wp->flags & SWP_NOSIZE) == 0)
        {
            wp->flags |= SWP_NOCOPYBITS;
            *result = ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
            return true;
        }
        return false;
    }
    case WM_NCACTIVATE:
    {
        *result = TRUE;
        return true;
    }
    case WM_NCCALCSIZE:
    {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 3) && QT_VERSION <= QT_VERSION_CHECK(6, 6, 1))
        if (wParam == FALSE)
        {
            return false;
        }
        if (::IsZoomed(hwnd))
        {
            setContentsMargins(8, 8, 8, 8);
        }
        else
        {
            setContentsMargins(0, 0, 0, 0);
        }
        *result = 0;
        return true;
#else
        if (wParam == FALSE)
        {
            return false;
        }
        RECT* clientRect = &((NCCALCSIZE_PARAMS*)(lParam))->rgrc[0];
        if (!::IsZoomed(hwnd))
        {
            clientRect->top -= 1;
            clientRect->bottom -= 1;
        }
        *result = WVR_REDRAW;
        return true;
#endif
    }
    }
    return QDialog::nativeEvent(eventType, message, result);
}
#endif


