#include "FingerprintManager.h"
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

FingerprintManager::FingerprintManager(QObject* parent)
    : QObject(parent)
{
}

void FingerprintManager::applyFingerprint(QWebEngineProfile* profile, const FingerprintConfig& config)
{
    if (!profile) return;
    
    // Set user agent
    profile->setHttpUserAgent(config.userAgent);
    
    // Create injection script
    QString script = generateInjectionScript(config);
    
    QWebEngineScript injectionScript;
    injectionScript.setName("fingerprint-spoofing");
    injectionScript.setSourceCode(script);
    injectionScript.setInjectionPoint(QWebEngineScript::DocumentCreation);
    injectionScript.setWorldId(QWebEngineScript::MainWorld);
    injectionScript.setRunsOnSubFrames(true);
    
    profile->scripts()->insert(injectionScript);
}

QString FingerprintManager::generateInjectionScript(const FingerprintConfig& config)
{
    QString script = "(function() {\n";
    script += "  'use strict';\n\n";
    
    script += generateTimezoneScript(config);
    script += generateNavigatorScript(config);
    script += generateScreenScript(config);
    script += generateCanvasScript(config);
    script += generateWebGLScript(config);
    script += generateAudioScript(config);
    script += generateWebRTCScript(config);
    
    script += "})();\n";
    
    return script;
}

QString FingerprintManager::generateNavigatorScript(const FingerprintConfig& config)
{
    return QString(R"(
  // Navigator properties
  Object.defineProperty(navigator, 'platform', {
    get: () => '%1'
  });
  
  Object.defineProperty(navigator, 'vendor', {
    get: () => '%2'
  });
  
  Object.defineProperty(navigator, 'hardwareConcurrency', {
    get: () => %3
  });
  
  Object.defineProperty(navigator, 'deviceMemory', {
    get: () => %4
  });
  
  Object.defineProperty(navigator, 'language', {
    get: () => '%5'
  });
  
  Object.defineProperty(navigator, 'languages', {
    get: () => ['%5']
  });
  
)").arg(config.platform)
   .arg(config.vendor)
   .arg(config.hardwareConcurrency)
   .arg(config.deviceMemory)
   .arg(config.locale);
}

QString FingerprintManager::generateScreenScript(const FingerprintConfig& config)
{
    return QString(R"(
  // Screen properties
  Object.defineProperty(screen, 'width', {
    get: () => %1
  });
  
  Object.defineProperty(screen, 'height', {
    get: () => %2
  });
  
  Object.defineProperty(screen, 'availWidth', {
    get: () => %1
  });
  
  Object.defineProperty(screen, 'availHeight', {
    get: () => %2 - 40
  });
  
  Object.defineProperty(screen, 'colorDepth', {
    get: () => %3
  });
  
  Object.defineProperty(screen, 'pixelDepth', {
    get: () => %3
  });
  
)").arg(config.screenWidth)
   .arg(config.screenHeight)
   .arg(config.colorDepth);
}

QString FingerprintManager::generateCanvasScript(const FingerprintConfig& config)
{
    if (!config.canvasNoise) {
        return "";
    }
    
    return R"(
  // Canvas fingerprint protection
  const originalToDataURL = HTMLCanvasElement.prototype.toDataURL;
  const originalToBlob = HTMLCanvasElement.prototype.toBlob;
  const originalGetImageData = CanvasRenderingContext2D.prototype.getImageData;
  
  const addNoise = (canvas, context) => {
    const imageData = context.getImageData(0, 0, canvas.width, canvas.height);
    for (let i = 0; i < imageData.data.length; i += 4) {
      imageData.data[i] = imageData.data[i] ^ Math.floor(Math.random() * 10);
    }
    context.putImageData(imageData, 0, 0);
  };
  
  HTMLCanvasElement.prototype.toDataURL = function(...args) {
    const context = this.getContext('2d');
    if (context) addNoise(this, context);
    return originalToDataURL.apply(this, args);
  };
  
  CanvasRenderingContext2D.prototype.getImageData = function(...args) {
    const imageData = originalGetImageData.apply(this, args);
    for (let i = 0; i < imageData.data.length; i += 4) {
      imageData.data[i] = imageData.data[i] ^ Math.floor(Math.random() * 10);
    }
    return imageData;
  };
  
)";
}

QString FingerprintManager::generateWebGLScript(const FingerprintConfig& config)
{
    return QString(R"(
  // WebGL fingerprint spoofing
  const getParameter = WebGLRenderingContext.prototype.getParameter;
  WebGLRenderingContext.prototype.getParameter = function(parameter) {
    if (parameter === 37445) {
      return '%1';
    }
    if (parameter === 37446) {
      return '%2';
    }
    return getParameter.apply(this, arguments);
  };
  
  const getParameter2 = WebGL2RenderingContext.prototype.getParameter;
  WebGL2RenderingContext.prototype.getParameter = function(parameter) {
    if (parameter === 37445) {
      return '%1';
    }
    if (parameter === 37446) {
      return '%2';
    }
    return getParameter2.apply(this, arguments);
  };
  
)").arg(config.webglVendor)
   .arg(config.webglRenderer);
}

QString FingerprintManager::generateAudioScript(const FingerprintConfig& config)
{
    if (!config.audioNoise) {
        return "";
    }
    
    return R"(
  // Audio Context fingerprint protection
  const audioContext = AudioContext.prototype.createAnalyser;
  AudioContext.prototype.createAnalyser = function() {
    const analyser = audioContext.apply(this, arguments);
    const getFloatFrequencyData = analyser.getFloatFrequencyData;
    analyser.getFloatFrequencyData = function(array) {
      getFloatFrequencyData.apply(this, arguments);
      for (let i = 0; i < array.length; i++) {
        array[i] += Math.random() * 0.0001;
      }
    };
    return analyser;
  };
  
)";
}

QString FingerprintManager::generateWebRTCScript(const FingerprintConfig& config)
{
    if (!config.webrtcProtection) {
        return "";
    }
    
    return R"(
  // WebRTC leak protection
  if (window.RTCPeerConnection) {
    const originalRTCPeerConnection = window.RTCPeerConnection;
    window.RTCPeerConnection = function(...args) {
      if (args[0] && args[0].iceServers) {
        args[0].iceServers = [];
      }
      return new originalRTCPeerConnection(...args);
    };
  }
  
  if (navigator.mediaDevices && navigator.mediaDevices.enumerateDevices) {
    navigator.mediaDevices.enumerateDevices = () => Promise.resolve([]);
  }
  
)";
}

QString FingerprintManager::generateTimezoneScript(const FingerprintConfig& config)
{
    if (config.timezone.isEmpty()) {
        return "";
    }
    
    return QString(R"(
  // Timezone spoofing - MUST BE FIRST!
  const targetTimezone = '%1';
  const timezoneOffsets = {
    'America/New_York': 300,
    'America/Chicago': 360,
    'America/Denver': 420,
    'America/Los_Angeles': 480,
    'America/Anchorage': 540,
    'Pacific/Honolulu': 600,
    'Europe/London': 0,
    'Europe/Paris': -60,
    'Europe/Berlin': -60,
    'Europe/Moscow': -180,
    'Asia/Dubai': -240,
    'Asia/Karachi': -300,
    'Asia/Dhaka': -360,
    'Asia/Bangkok': -420,
    'Asia/Shanghai': -480,
    'Asia/Tokyo': -540,
    'Australia/Sydney': -600,
    'Pacific/Auckland': -720
  };
  const targetOffset = timezoneOffsets[targetTimezone] || 0;
  
  // Override Date.prototype.getTimezoneOffset
  const originalGetTimezoneOffset = Date.prototype.getTimezoneOffset;
  Date.prototype.getTimezoneOffset = function() {
    return targetOffset;
  };
  
  // Override Date.prototype.toString
  const originalToString = Date.prototype.toString;
  Date.prototype.toString = function() {
    const offset = targetOffset;
    const absOffset = Math.abs(offset);
    const hours = Math.floor(absOffset / 60);
    const minutes = absOffset %% 60;
    const sign = offset <= 0 ? '+' : '-';
    const tzStr = `GMT${sign}${String(hours).padStart(2, '0')}${String(minutes).padStart(2, '0')}`;
    
    const utcTime = this.getTime();
    const localTime = new Date(utcTime - (offset * 60000));
    
    const days = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
    const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
    
    const day = days[localTime.getUTCDay()];
    const month = months[localTime.getUTCMonth()];
    const date = String(localTime.getUTCDate()).padStart(2, '0');
    const year = localTime.getUTCFullYear();
    const hour = String(localTime.getUTCHours()).padStart(2, '0');
    const minute = String(localTime.getUTCMinutes()).padStart(2, '0');
    const second = String(localTime.getUTCSeconds()).padStart(2, '0');
    
    const tzAbbr = targetTimezone.includes('Los_Angeles') ? 'PST' : 
                   targetTimezone.includes('New_York') ? 'EST' :
                   targetTimezone.includes('Chicago') ? 'CST' :
                   targetTimezone.includes('Denver') ? 'MST' :
                   targetTimezone.split('/').pop().replace(/_/g, ' ');
    return `${day} ${month} ${date} ${year} ${hour}:${minute}:${second} ${tzStr} (${tzAbbr})`;
  };
  
  // Override Date.prototype.toTimeString
  const originalToTimeString = Date.prototype.toTimeString;
  Date.prototype.toTimeString = function() {
    const offset = targetOffset;
    const absOffset = Math.abs(offset);
    const hours = Math.floor(absOffset / 60);
    const minutes = absOffset %% 60;
    const sign = offset <= 0 ? '+' : '-';
    const tzStr = `GMT${sign}${String(hours).padStart(2, '0')}${String(minutes).padStart(2, '0')}`;
    
    const utcTime = this.getTime();
    const localTime = new Date(utcTime - (offset * 60000));
    
    const hour = String(localTime.getUTCHours()).padStart(2, '0');
    const minute = String(localTime.getUTCMinutes()).padStart(2, '0');
    const second = String(localTime.getUTCSeconds()).padStart(2, '0');
    
    const tzName = targetTimezone.split('/').pop().replace(/_/g, ' ');
    return `${hour}:${minute}:${second} ${tzStr} (${tzName})`;
  };
  
  // Override Date.prototype.toLocaleString
  const originalToLocaleString = Date.prototype.toLocaleString;
  Date.prototype.toLocaleString = function(locales, options) {
    options = options || {};
    options.timeZone = targetTimezone;
    return originalToLocaleString.call(this, locales, options);
  };
  
  // Override Date.prototype.toLocaleDateString
  const originalToLocaleDateString = Date.prototype.toLocaleDateString;
  Date.prototype.toLocaleDateString = function(locales, options) {
    options = options || {};
    options.timeZone = targetTimezone;
    return originalToLocaleDateString.call(this, locales, options);
  };
  
  // Override Date.prototype.toLocaleTimeString
  const originalToLocaleTimeString = Date.prototype.toLocaleTimeString;
  Date.prototype.toLocaleTimeString = function(locales, options) {
    options = options || {};
    options.timeZone = targetTimezone;
    return originalToLocaleTimeString.call(this, locales, options);
  };
  
  // Override Intl.DateTimeFormat
  const OriginalDateTimeFormat = Intl.DateTimeFormat;
  Intl.DateTimeFormat = function(locales, options) {
    options = options || {};
    if (!options.timeZone) {
      options.timeZone = targetTimezone;
    }
    return new OriginalDateTimeFormat(locales, options);
  };
  Intl.DateTimeFormat.prototype = OriginalDateTimeFormat.prototype;
  
  // Override Intl.DateTimeFormat.prototype.resolvedOptions
  const originalResolvedOptions = OriginalDateTimeFormat.prototype.resolvedOptions;
  Intl.DateTimeFormat.prototype.resolvedOptions = function() {
    const options = originalResolvedOptions.call(this);
    options.timeZone = targetTimezone;
    return options;
  };
  
)").arg(config.timezone);
}
