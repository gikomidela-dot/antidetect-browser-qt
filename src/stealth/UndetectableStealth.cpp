#include "UndetectableStealth.h"
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

void UndetectableStealth::applyStealthMode(QWebEnginePage* page)
{
    if (!page) return;
    
    // Inject stealth script before page load
    QWebEngineScript script;
    script.setName("undetectable-stealth");
    script.setSourceCode(getStealthScript());
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    
    page->scripts().insert(script);
}

void UndetectableStealth::applyStealthProfile(QWebEngineProfile* profile)
{
    if (!profile) return;
    
    // Set user agent to real Chrome
    profile->setHttpUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
    
    // Disable automation flags
    profile->setHttpAcceptLanguage("en-US,en;q=0.9");
}

QString UndetectableStealth::getStealthScript()
{
    return getChromeDriverPatch() + "\n" +
           getNavigatorPatch() + "\n" +
           getWebGLPatch() + "\n" +
           getCanvasPatch() + "\n" +
           getPermissionsPatch() + "\n" +
           getPluginsPatch() + "\n" +
           getWebRTCPatch();
}

QString UndetectableStealth::getChromeDriverPatch()
{
    return R"(
// Remove webdriver property
Object.defineProperty(navigator, 'webdriver', {
    get: () => undefined
});

// Remove automation flags
delete navigator.__proto__.webdriver;

// Patch chrome object
if (!window.chrome) {
    window.chrome = {};
}

window.chrome.runtime = {
    connect: function() {},
    sendMessage: function() {}
};

// Remove cdc_ properties (ChromeDriver detection)
const originalQuery = window.document.querySelector;
window.document.querySelector = function(selector) {
    if (selector && selector.includes('cdc_')) {
        return null;
    }
    return originalQuery.apply(this, arguments);
};
)";
}

QString UndetectableStealth::getNavigatorPatch()
{
    return R"(
// Fix navigator.plugins
Object.defineProperty(navigator, 'plugins', {
    get: () => [
        {
            0: {type: "application/x-google-chrome-pdf", suffixes: "pdf", description: "Portable Document Format"},
            description: "Portable Document Format",
            filename: "internal-pdf-viewer",
            length: 1,
            name: "Chrome PDF Plugin"
        },
        {
            0: {type: "application/pdf", suffixes: "pdf", description: "Portable Document Format"},
            description: "Portable Document Format",
            filename: "mhjfbmdgcfjbbpaeojofohoefgiehjai",
            length: 1,
            name: "Chrome PDF Viewer"
        },
        {
            0: {type: "application/x-nacl", suffixes: "", description: "Native Client Executable"},
            1: {type: "application/x-pnacl", suffixes: "", description: "Portable Native Client Executable"},
            description: "",
            filename: "internal-nacl-plugin",
            length: 2,
            name: "Native Client"
        }
    ]
});

// Fix navigator.languages
Object.defineProperty(navigator, 'languages', {
    get: () => ['en-US', 'en']
});

// Fix navigator.permissions
const originalQuery = window.navigator.permissions.query;
window.navigator.permissions.query = (parameters) => (
    parameters.name === 'notifications' ?
        Promise.resolve({ state: Notification.permission }) :
        originalQuery(parameters)
);
)";
}

QString UndetectableStealth::getWebGLPatch()
{
    return R"(
// WebGL vendor spoofing
const getParameter = WebGLRenderingContext.prototype.getParameter;
WebGLRenderingContext.prototype.getParameter = function(parameter) {
    if (parameter === 37445) {
        return 'Intel Inc.';
    }
    if (parameter === 37446) {
        return 'Intel Iris OpenGL Engine';
    }
    return getParameter.apply(this, arguments);
};

const getParameter2 = WebGL2RenderingContext.prototype.getParameter;
WebGL2RenderingContext.prototype.getParameter = function(parameter) {
    if (parameter === 37445) {
        return 'Intel Inc.';
    }
    if (parameter === 37446) {
        return 'Intel Iris OpenGL Engine';
    }
    return getParameter2.apply(this, arguments);
};
)";
}

QString UndetectableStealth::getCanvasPatch()
{
    return R"(
// Canvas fingerprint protection
const toBlob = HTMLCanvasElement.prototype.toBlob;
const toDataURL = HTMLCanvasElement.prototype.toDataURL;
const getImageData = CanvasRenderingContext2D.prototype.getImageData;

const noisify = function(canvas, context) {
    const shift = {
        'r': Math.floor(Math.random() * 10) - 5,
        'g': Math.floor(Math.random() * 10) - 5,
        'b': Math.floor(Math.random() * 10) - 5,
        'a': Math.floor(Math.random() * 10) - 5
    };
    
    const width = canvas.width;
    const height = canvas.height;
    const imageData = getImageData.apply(context, [0, 0, width, height]);
    
    for (let i = 0; i < height; i++) {
        for (let j = 0; j < width; j++) {
            const n = ((i * (width * 4)) + (j * 4));
            imageData.data[n + 0] = imageData.data[n + 0] + shift.r;
            imageData.data[n + 1] = imageData.data[n + 1] + shift.g;
            imageData.data[n + 2] = imageData.data[n + 2] + shift.b;
            imageData.data[n + 3] = imageData.data[n + 3] + shift.a;
        }
    }
    
    context.putImageData(imageData, 0, 0);
};

Object.defineProperty(HTMLCanvasElement.prototype, 'toBlob', {
    value: function() {
        noisify(this, this.getContext('2d'));
        return toBlob.apply(this, arguments);
    }
});

Object.defineProperty(HTMLCanvasElement.prototype, 'toDataURL', {
    value: function() {
        noisify(this, this.getContext('2d'));
        return toDataURL.apply(this, arguments);
    }
});
)";
}

QString UndetectableStealth::getPermissionsPatch()
{
    return R"(
// Permissions API
const originalPermissionsQuery = window.navigator.permissions.query;
window.navigator.permissions.query = function(parameters) {
    if (parameters.name === 'notifications') {
        return Promise.resolve({
            state: Notification.permission,
            onchange: null
        });
    }
    return originalPermissionsQuery.apply(this, arguments);
};
)";
}

QString UndetectableStealth::getPluginsPatch()
{
    return R"(
// Fix mimeTypes
Object.defineProperty(navigator, 'mimeTypes', {
    get: () => [
        {type: "application/pdf", suffixes: "pdf", description: "Portable Document Format"},
        {type: "application/x-google-chrome-pdf", suffixes: "pdf", description: "Portable Document Format"},
        {type: "application/x-nacl", suffixes: "", description: "Native Client Executable"},
        {type: "application/x-pnacl", suffixes: "", description: "Portable Native Client Executable"}
    ]
});

// Fix platform
Object.defineProperty(navigator, 'platform', {
    get: () => 'Win32'
});

// Fix hardwareConcurrency
Object.defineProperty(navigator, 'hardwareConcurrency', {
    get: () => 8
});

// Fix deviceMemory
Object.defineProperty(navigator, 'deviceMemory', {
    get: () => 8
});
)";
}

QString UndetectableStealth::getWebRTCPatch()
{
    return R"(
// WebRTC IP leak protection
const originalRTCPeerConnection = window.RTCPeerConnection;
window.RTCPeerConnection = function(...args) {
    const pc = new originalRTCPeerConnection(...args);
    
    const originalCreateOffer = pc.createOffer;
    pc.createOffer = function() {
        const offer = originalCreateOffer.apply(this, arguments);
        return offer.then(function(description) {
            description.sdp = description.sdp.replace(/([0-9]{1,3}(\.[0-9]{1,3}){3}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){7})/g, '0.0.0.0');
            return description;
        });
    };
    
    return pc;
};

// Prevent WebRTC leaks
Object.defineProperty(window, 'RTCPeerConnection', {
    value: window.RTCPeerConnection,
    writable: false,
    configurable: false
});
)";
}
