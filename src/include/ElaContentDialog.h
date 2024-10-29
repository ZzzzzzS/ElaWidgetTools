#ifndef ELACONTENTDIALOG_H
#define ELACONTENTDIALOG_H
#include <QAbstractNativeEventFilter>
#include <QDialog>

#include "stdafx.h"

class ElaContentDialogPrivate;
class ElaPushButton;
class ElaText;

class ELA_EXPORT ElaContentDialog : public QDialog
{
    Q_OBJECT
    Q_Q_CREATE(ElaContentDialog)
public:
    explicit ElaContentDialog(QWidget* parent,const QString& title=QString(),const QString& subtitle=QString());
    ~ElaContentDialog();
    Q_SLOT virtual void onLeftButtonClicked();
    Q_SLOT virtual void onMiddleButtonClicked();
    Q_SLOT virtual void onRightButtonClicked();
    void setCentralWidget(QWidget* centralWidget);
    void setButtonNumber(size_t number);
    void setTitleSubTitle(const QString& Title, const QString SubTitle);

    void setLeftButtonText(QString text, bool heightlight = false);
    void setMiddleButtonText(QString text, bool heightlight = false);
    void setRightButtonText(QString text, bool heightlight = false);
Q_SIGNALS:
    Q_SIGNAL void leftButtonClicked();
    Q_SIGNAL void middleButtonClicked();
    Q_SIGNAL void rightButtonClicked();

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#else
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#endif
#endif
private:
    void SetButtonHeightLight(ElaPushButton* button, bool heightlight);
    bool CentralWidgetChanged;
    ElaText* Title;
    ElaText* SubTitle;
};

#endif // ELACONTENTDIALOG_H
