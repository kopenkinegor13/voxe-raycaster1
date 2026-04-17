#!/bin/bash

# Voxel Engine - Автоматический сборщик
# Этот скрипт проверит зависимости, установит их (если нужно) и соберет проект.

echo "🚀 Запуск автоматической сборки Voxel Engine..."

# Определение ОС
if [ "$(uname)" == "Darwin" ]; then
    echo "🍎 Обнаружена macOS"
    # Проверка Homebrew
    if ! command -v brew &> /dev/null; then
        echo "❌ Homebrew не найден. Пожалуйста, установите его вручную: https://brew.sh"
        exit 1
    fi
    # Установка зависимостей
    echo "📦 Установка зависимостей через Homebrew..."
    brew install sdl2 glew
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    echo "🐧 Обнаружена Linux"
    # Проверка менеджера пакетов
    if command -v apt-get &> /dev/null; then
        echo "📦 Установка зависимостей через apt..."
        sudo apt-get update
        sudo apt-get install -y build-essential libsdl2-dev libglew-dev libgl1-mesa-dev
    elif command -v dnf &> /dev/null; then
        echo "📦 Установка зависимостей через dnf..."
        sudo dnf install -y gcc-c++ SDL2-devel GLEW-devel mesa-libGL-devel make
    elif command -v pacman &> /dev/null; then
        echo "📦 Установка зависимостей через pacman..."
        sudo pacman -S --noconfirm base-devel sdl2 glew mesa
    else
        echo "⚠️ Неизвестный менеджер пакетов. Пожалуйста, установите зависимости вручную:"
        echo "   - SDL2"
        echo "   - GLEW"
        echo "   - OpenGL/Mesa"
        echo "   - Make и GCC"
        exit 1
    fi
else
    echo "❌ Неподдерживаемая ОС. Требуется Linux или macOS."
    exit 1
fi

# Сборка проекта
echo "🔨 Компиляция проекта..."
make clean
make

if [ $? -eq 0 ]; then
    echo "✅ Сборка успешна!"
    echo "🎮 Для запуска игры выполните: ./bin/voxel-gl"
    echo "   или используйте команду: make run"
else
    echo "❌ Ошибка при сборке. Проверьте сообщения выше."
    exit 1
fi
