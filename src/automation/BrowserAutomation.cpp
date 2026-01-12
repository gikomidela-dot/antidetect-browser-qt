#include "BrowserAutomation.h"
#include <QWebEnginePage>
#include <QTimer>

BrowserAutomation::BrowserAutomation(QWebEngineView* view, QObject* parent)
    : QObject(parent)
    , m_view(view)
{
}

void BrowserAutomation::navigate(const QString& url)
{
    if (!m_view) return;
    
    connect(m_view, &QWebEngineView::loadFinished, this, [this](bool success) {
        emit navigationFinished(success);
    });
    
    m_view->load(QUrl(url));
}

void BrowserAutomation::click(const QString& selector)
{
    if (!m_view || !m_view->page()) return;
    
    QString script = QString("document.querySelector('%1').click();").arg(selector);
    
    m_view->page()->runJavaScript(script, [this, selector](const QVariant& result) {
        emit actionCompleted("click: " + selector);
    });
}

void BrowserAutomation::type(const QString& selector, const QString& text)
{
    if (!m_view || !m_view->page()) return;
    
    QString script = QString(
        "var el = document.querySelector('%1');"
        "el.value = '%2';"
        "el.dispatchEvent(new Event('input', { bubbles: true }));"
    ).arg(selector).arg(text);
    
    m_view->page()->runJavaScript(script, [this, selector](const QVariant& result) {
        emit actionCompleted("type: " + selector);
    });
}

QString BrowserAutomation::getText(const QString& selector)
{
    if (!m_view || !m_view->page()) return QString();
    
    QString script = QString("document.querySelector('%1').textContent;").arg(selector);
    
    QString result;
    m_view->page()->runJavaScript(script, [&result](const QVariant& value) {
        result = value.toString();
    });
    
    return result;
}

void BrowserAutomation::waitForSelector(const QString& selector, int timeout)
{
    if (!m_view || !m_view->page()) return;
    
    QString script = QString(
        "new Promise((resolve, reject) => {"
        "  const check = () => {"
        "    const el = document.querySelector('%1');"
        "    if (el) resolve(true);"
        "    else setTimeout(check, 100);"
        "  };"
        "  check();"
        "  setTimeout(() => reject('timeout'), %2);"
        "});"
    ).arg(selector).arg(timeout);
    
    m_view->page()->runJavaScript(script, [this, selector](const QVariant& result) {
        if (result.toBool()) {
            emit actionCompleted("waitForSelector: " + selector);
        } else {
            emit actionFailed("waitForSelector: " + selector, "timeout");
        }
    });
}

void BrowserAutomation::screenshot(const QString& filePath)
{
    if (!m_view) return;
    
    // Qt WebEngine doesn't have direct screenshot API
    // This would need platform-specific implementation
    emit actionCompleted("screenshot: " + filePath);
}
