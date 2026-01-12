#include "AudioSpoofing.h"

QString AudioSpoofing::generateScript(bool enabled)
{
    if (!enabled) return "";
    
    return R"(
// Audio Context spoofing
(function() {
    const audioContext = AudioContext.prototype.createAnalyser;
    AudioContext.prototype.createAnalyser = function() {
        const analyser = audioContext.call(this);
        const getFloatFrequencyData = analyser.getFloatFrequencyData;
        analyser.getFloatFrequencyData = function(array) {
            getFloatFrequencyData.call(this, array);
            for (let i = 0; i < array.length; i++) {
                array[i] += Math.random() * 0.001;
            }
        };
        return analyser;
    };
})();
)";
}
