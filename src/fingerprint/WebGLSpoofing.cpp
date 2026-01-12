#include "WebGLSpoofing.h"

QString WebGLSpoofing::generateScript(const QString& vendor, const QString& renderer)
{
    return QString(R"(
// WebGL spoofing
(function() {
    const getParameter = WebGLRenderingContext.prototype.getParameter;
    WebGLRenderingContext.prototype.getParameter = function(parameter) {
        if (parameter === 37445) return '%1';
        if (parameter === 37446) return '%2';
        return getParameter.call(this, parameter);
    };
})();
)").arg(vendor).arg(renderer);
}
