#!/bin/bash

echo "=== SECTA Anti Detect - Исправление зависимостей ==="
echo ""

# Определяем дистрибутив
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "Не могу определить дистрибутив"
    exit 1
fi

echo "Обнаружен дистрибутив: $OS"
echo ""

case $OS in
    ubuntu|debian)
        echo "Устанавливаю Qt6 для Ubuntu/Debian..."
        sudo apt update
        
        # Проверяем версию Ubuntu
        VERSION=$(lsb_release -rs)
        echo "Версия Ubuntu: $VERSION"
        
        if (( $(echo "$VERSION >= 24.04" | bc -l) )); then
            echo "Ubuntu 24.04+, устанавливаю Qt6..."
            sudo apt install -y qt6-base-dev qt6-webengine-dev \
                libqt6webenginecore6 libqt6webenginewidgets6 \
                libqt6core6 libqt6gui6 libqt6widgets6 libqt6network6
        else
            echo "Ubuntu < 24.04, нужна более новая версия Qt"
            echo ""
            echo "Вариант 1: Обновите Ubuntu до 24.04"
            echo "Вариант 2: Установите Qt6 из PPA:"
            echo "  sudo add-apt-repository ppa:okirby/qt6-backports"
            echo "  sudo apt update"
            echo "  sudo apt install qt6-base-dev qt6-webengine-dev"
            echo ""
            echo "Вариант 3: Скачайте Windows версию и запустите в Wine"
        fi
        ;;
        
    arch|manjaro)
        echo "Устанавливаю Qt6 для Arch/Manjaro..."
        sudo pacman -Sy --noconfirm qt6-base qt6-webengine
        ;;
        
    fedora)
        echo "Устанавливаю Qt6 для Fedora..."
        sudo dnf install -y qt6-qtbase qt6-qtwebengine
        ;;
        
    *)
        echo "Неизвестный дистрибутив: $OS"
        echo "Установите Qt6 вручную"
        ;;
esac

echo ""
echo "=== Готово! ==="
echo "Теперь попробуйте запустить: ./SECTAAntiDetect"
