#include "CanvasSpoofing.h"

QString CanvasSpoofing::generateScript(bool enabled)
{
    if (!enabled) return "";
    
    return R"(
// Canvas noise injection
(function() {
    const noisify = (canvas, context) => {
        const shift = Math.floor(Math.random() * 10) - 5;
        const imageData = context.getImageData(0, 0, canvas.width, canvas.height);
        for (let i = 0; i < imageData.data.length; i++) {
            imageData.data[i] = imageData.data[i] + shift;
        }
        context.putImageData(imageData, 0, 0);
    };
    
    const toDataURL = HTMLCanvasElement.prototype.toDataURL;
    HTMLCanvasElement.prototype.toDataURL = function(type) {
        noisify(this, this.getContext('2d'));
        return toDataURL.apply(this, arguments);
    };
})();
)";
}
