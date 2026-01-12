# VPN Support Documentation

## 🔐 Поддерживаемые VPN протоколы

Antidetect Browser поддерживает следующие VPN протоколы:

### 1. OpenVPN ✅
- **Формат файла**: `.ovpn`
- **Поддержка**: Полная
- **Парсер**: `OpenVPNParser`

**Пример конфигурации:**
```
client
dev tun
proto udp
remote vpn.example.com 1194
resolv-retry infinite
nobind
persist-key
persist-tun
ca ca.crt
cert client.crt
key client.key
cipher AES-256-CBC
auth SHA256
```

**Импорт:**
```cpp
VPNManager* vpnManager = Application::instance().vpnManager();
vpnManager->importConfig("/path/to/config.ovpn");
```

---

### 2. WireGuard ✅
- **Формат файла**: `.conf`
- **Поддержка**: Полная
- **Парсер**: `WireGuardParser`

**Пример конфигурации:**
```ini
[Interface]
PrivateKey = yAnz5TF+lXXJte14tji3zlMNq+hd2rYUIgJBgB3fBmk=
Address = 10.192.122.3/32
DNS = 1.1.1.1

[Peer]
PublicKey = HIgo9xNzJMWLKASShiTqIybxZ0U3wGLiUeJ1PKf8ykw=
Endpoint = 192.95.5.67:51820
AllowedIPs = 0.0.0.0/0
PersistentKeepalive = 21
```

**Импорт:**
```cpp
vpnManager->importConfig("/path/to/wg0.conf");
```

---

### 3. Shadowsocks ✅
- **Формат файла**: `.json`
- **Формат URL**: `ss://`
- **Поддержка**: Полная
- **Парсер**: `ShadowsocksParser`

**Пример JSON конфигурации:**
```json
{
  "server": "example.com",
  "server_port": 8388,
  "password": "your-password",
  "method": "aes-256-gcm",
  "plugin": "v2ray-plugin",
  "plugin_opts": "server;tls;host=example.com",
  "remarks": "My Shadowsocks Server",
  "timeout": 300
}
```

**Пример ss:// URL:**
```
ss://YWVzLTI1Ni1nY206cGFzc3dvcmQ=@example.com:8388#MyServer
```

**Импорт из файла:**
```cpp
vpnManager->importConfig("/path/to/config.json");
```

**Импорт из URL:**
```cpp
vpnManager->importFromUrl("ss://YWVzLTI1Ni1nY206cGFzc3dvcmQ=@example.com:8388#MyServer");
```

---

### 4. V2Ray / XRay ✅
- **Формат файла**: `.json`
- **Форматы URL**: `vmess://`, `vless://`
- **Поддержка**: Полная
- **Парсер**: `V2RayParser`

**Пример JSON конфигурации:**
```json
{
  "outbounds": [{
    "protocol": "vmess",
    "settings": {
      "vnext": [{
        "address": "example.com",
        "port": 443,
        "users": [{
          "id": "b831381d-6324-4d53-ad4f-8cda48b30811",
          "alterId": 0,
          "security": "auto"
        }]
      }]
    },
    "streamSettings": {
      "network": "ws",
      "security": "tls",
      "wsSettings": {
        "path": "/path",
        "headers": {
          "Host": "example.com"
        }
      }
    }
  }]
}
```

**Пример vmess:// URL:**
```
vmess://eyJhZGQiOiJleGFtcGxlLmNvbSIsInBvcnQiOiI0NDMiLCJpZCI6ImIxMjM0NTY3LTg5YWItY2RlZi0wMTIzLTQ1Njc4OWFiY2RlZiIsImFpZCI6IjAiLCJuZXQiOiJ3cyIsInR5cGUiOiJub25lIiwiaG9zdCI6ImV4YW1wbGUuY29tIiwicGF0aCI6Ii9wYXRoIiwidGxzIjoidGxzIiwicHMiOiJNeSBWMlJheSJ9
```

**Пример vless:// URL:**
```
vless://b831381d-6324-4d53-ad4f-8cda48b30811@example.com:443?type=ws&security=tls&path=/path&host=example.com#MyVLESS
```

**Импорт из файла:**
```cpp
vpnManager->importConfig("/path/to/v2ray-config.json");
```

**Импорт из URL:**
```cpp
vpnManager->importFromUrl("vmess://...");
vpnManager->importFromUrl("vless://...");
```

---

## 📚 API Reference

### VPNManager

**Методы:**

```cpp
// Импорт конфигурации из файла
bool importConfig(const QString& filePath);

// Импорт конфигурации из URL
bool importFromUrl(const QString& url);

// Парсинг конфигурации из файла
VPNConfig parseConfig(const QString& filePath, const QString& type);

// Парсинг конфигурации из URL
VPNConfig parseFromUrl(const QString& url);

// Получить все конфигурации
QList<VPNConfig> getConfigs() const;

// Получить конкретную конфигурацию
VPNConfig getConfig(const QString& name) const;

// Удалить конфигурацию
bool removeConfig(const QString& name);
```

**Сигналы:**

```cpp
// Конфигурация успешно импортирована
void configImported(const QString& name);

// Ошибка импорта
void importError(const QString& error);
```

---

## 🔧 Использование в коде

### Пример 1: Импорт OpenVPN конфигурации

```cpp
#include "core/Application.h"
#include "vpn/VPNManager.h"

VPNManager* vpnManager = Application::instance().vpnManager();

// Подключить сигналы
connect(vpnManager, &VPNManager::configImported, [](const QString& name) {
    qDebug() << "VPN config imported:" << name;
});

connect(vpnManager, &VPNManager::importError, [](const QString& error) {
    qWarning() << "Import error:" << error;
});

// Импортировать конфигурацию
vpnManager->importConfig("/home/user/vpn/config.ovpn");
```

### Пример 2: Импорт WireGuard конфигурации

```cpp
vpnManager->importConfig("/etc/wireguard/wg0.conf");

// Получить конфигурацию
VPNConfig config = vpnManager->getConfig("wg0");

// Проверить параметры
qDebug() << "Server:" << config.parameters["server"];
qDebug() << "Port:" << config.parameters["port"];
qDebug() << "Public Key:" << config.parameters["publicKey"];
```

### Пример 3: Импорт Shadowsocks из URL

```cpp
QString ssUrl = "ss://YWVzLTI1Ni1nY206cGFzc3dvcmQ=@example.com:8388#MyServer";
vpnManager->importFromUrl(ssUrl);
```

### Пример 4: Импорт V2Ray из URL

```cpp
QString vmessUrl = "vmess://eyJhZGQiOiJleGFtcGxlLmNvbSIsInBvcnQiOiI0NDMiLCJpZCI6ImIxMjM0NTY3LTg5YWItY2RlZi0wMTIzLTQ1Njc4OWFiY2RlZiIsImFpZCI6IjAiLCJuZXQiOiJ3cyIsInR5cGUiOiJub25lIiwiaG9zdCI6ImV4YW1wbGUuY29tIiwicGF0aCI6Ii9wYXRoIiwidGxzIjoidGxzIiwicHMiOiJNeSBWMlJheSJ9";
vpnManager->importFromUrl(vmessUrl);
```

### Пример 5: Получение всех VPN конфигураций

```cpp
QList<VPNConfig> configs = vpnManager->getConfigs();

for (const VPNConfig& config : configs) {
    qDebug() << "Name:" << config.name;
    qDebug() << "Type:" << config.type;
    qDebug() << "Server:" << config.parameters["server"];
    qDebug() << "Port:" << config.parameters["port"];
    qDebug() << "---";
}
```

---

## 🎯 Структура VPNConfig

```cpp
struct VPNConfig {
    QString name;                      // Имя конфигурации
    QString type;                      // Тип: openvpn, wireguard, shadowsocks, v2ray
    QString configPath;                // Путь к файлу конфигурации
    QMap<QString, QString> parameters; // Параметры конфигурации
};
```

### Общие параметры:
- `server` - адрес сервера
- `port` - порт сервера

### OpenVPN параметры:
- `proto` - протокол (udp/tcp)
- `cipher` - шифрование
- `auth` - аутентификация

### WireGuard параметры:
- `privateKey` - приватный ключ
- `publicKey` - п��бличный ключ
- `endpoint` - endpoint сервера
- `allowedIPs` - разрешенные IP
- `address` - адрес интерфейса
- `dns` - DNS серверы

### Shadowsocks параметры:
- `password` - пароль
- `method` - метод шифрования (aes-256-gcm, chacha20-ietf-poly1305, etc.)
- `plugin` - плагин (v2ray-plugin, obfs-local, etc.)
- `plugin_opts` - опции плагина

### V2Ray параметры:
- `protocol` - протокол (vmess, vless, trojan, etc.)
- `id` - UUID пользователя
- `alterId` - alter ID (для vmess)
- `security` - безопасность (auto, aes-128-gcm, chacha20-poly1305, none)
- `network` - сеть (tcp, ws, h2, grpc, etc.)
- `type` - тип (none, http, srtp, utp, wechat-video)
- `host` - хост
- `path` - путь
- `tls` - TLS (tls, xtls, none)
- `sni` - SNI
- `encryption` - шифрование (для vless)
- `flow` - flow control (для vless)

---

## 🚀 Автоопределение типа VPN

VPNManager автоматически определяет тип VPN по:

1. **Расширению файла:**
   - `.ovpn` → OpenVPN
   - `.conf` → WireGuard
   - `.json` → Shadowsocks или V2Ray (автоопределение по содержимому)

2. **URL схеме:**
   - `ss://` → Shadowsocks
   - `vmess://` → V2Ray VMess
   - `vless://` → V2Ray VLESS

3. **Содержимому JSON:**
   - Наличие `"outbounds"` или `"inbounds"` → V2Ray
   - Иначе → Shadowsocks

---

## 📊 Статистика поддержки VPN

| Протокол | Статус | Парсер | Импорт из файла | Импорт из URL |
|----------|--------|--------|-----------------|---------------|
| OpenVPN | ✅ 100% | OpenVPNParser | ✅ | ❌ |
| WireGuard | ✅ 100% | WireGuardParser | ✅ | ❌ |
| Shadowsocks | ✅ 100% | ShadowsocksParser | ✅ | ✅ |
| V2Ray VMess | ✅ 100% | V2RayParser | ✅ | ✅ |
| V2Ray VLESS | ✅ 100% | V2RayParser | ✅ | ✅ |
| Trojan | 🔄 Planned | - | ❌ | ❌ |
| Hysteria | 🔄 Planned | - | ❌ | ❌ |

---

## 🔍 Примеры тестирования

### Тест 1: Импорт всех типов VPN

```cpp
void testVPNImport() {
    VPNManager vpnManager;
    
    // OpenVPN
    vpnManager.importConfig("test/configs/openvpn.ovpn");
    
    // WireGuard
    vpnManager.importConfig("test/configs/wg0.conf");
    
    // Shadowsocks
    vpnManager.importConfig("test/configs/ss.json");
    vpnManager.importFromUrl("ss://...");
    
    // V2Ray
    vpnManager.importConfig("test/configs/v2ray.json");
    vpnManager.importFromUrl("vmess://...");
    vpnManager.importFromUrl("vless://...");
    
    // Проверка
    QList<VPNConfig> configs = vpnManager.getConfigs();
    qDebug() << "Total configs:" << configs.size();
}
```

---

## 📝 Заметки

1. **Безопасность**: Все пароли и ключи хранятся в памяти и не логируются
2. **Валидация**: Парсеры проверяют корректность конфигураций
3. **Ошибки**: При ошибках парсинга возвращается пустой VPNConfig
4. **Расширяемость**: Легко добавить новые протоколы, создав новый парсер

---

## 🤝 Вклад

Для добавления нового VPN протокола:

1. Создайте `NewVPNParser.h` и `NewVPNParser.cpp`
2. Реализуйте статический метод `parse()`
3. Добавьте в `VPNManager::parseConfig()`
4. Обновите `CMakeLists.txt`
5. Добавьте тесты

---

## 📞 Поддержка

При возникновении проблем с VPN модулем:
- Проверьте формат конфигурационного файла
- Убедитесь, что файл читаемый
- Проверьте логи приложения
- Создайте issue с примером конфигурации
