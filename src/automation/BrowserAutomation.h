#ifndef BROWSERAUTOMATION_H
#define BROWSERAUTOMATION_H

#include <QObject>
#include <QWebEngineView>

class BrowserAutomation : public QObject
{
    Q_OBJECT
    
public:
    explicit BrowserAutomation(QWebEngineView* view, QObject* parent = nullptr);
    
    // Puppeteer-like API
    Q_INVOKABLE void navigate(const QString& url);
    Q_INVOKABLE void click(const QString& selector);
    Q_INVOKABLE void type(const QString& selector, const QString& text);
    Q_INVOKABLE QString getText(const QString& selector);
    Q_INVOKABLE void waitForSelector(const QString& selector, int timeout = 30000);
    Q_INVOKABLE void screenshot(const QString& filePath);
    
signals:
    void navigationFinished(bool success);
    void actionCompleted(const QString& action);
    void actionFailed(const QString& action, const QString& error);
    
private:
    QWebEngineView* m_view;
};

#endif // BROWSERAUTOMATION_H
