<div align="center">

# PRTS DevOps Платформа

<img src="assets/logo.png" alt="PRTS Logo" width="120" height="120">

**Корпоративная платформа управления DevOps**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### Выбор языка

[**English**](../README.md) | [**中文**](README.zh-CN.md) | [**日本語**](README.ja.md) | [**Русский**](README.ru.md) | [**Français**](README.fr.md) | [**Deutsch**](README.de.md)

</div>

---

## Обзор

PRTS (Production Resource Tracking System) — это корпоративная платформа управления DevOps, включающая:

- **Logos IDE** — IDE на базе Electron с Vue 3 + MDUI, редактором Monaco и интеллектуальным анализом кода
- **Мобильные приложения** — нативные приложения для iOS (SwiftUI) и Android (Flutter) с push-уведомлениями
- **Backend API** — асинхронный Python FastAPI, очередь задач Celery и комплексный мониторинг
- **Нативный слой производительности** — модули на C для высокопроизводительного сбора метрик и обработки логов

> *Название «Logos» происходит от элитного оператора в игре «Arknights» (明日方舟)*

## Основные возможности

### Logos IDE
- Редактор Monaco с интеллектуальным автодополнением
- Встроенный терминал (xterm.js)
- Управление CI/CD пайплайнами
- Панель мониторинга в реальном времени
- Современный интерфейс на базе MDUI

### Мобильные приложения
- **iOS**: SwiftUI, push-уведомления APNs, Live Activity, Dynamic Island
- **Android**: Flutter, Material 3, Firebase Cloud Messaging

### Серверные сервисы
- RESTful API на FastAPI
- JWT-аутентификация
- PostgreSQL + Redis
- Асинхронные задачи через Celery
- Метрики Prometheus

### Нативный слой производительности
- Пул памяти для снижения накладных расходов на выделение
- Пул потоков для параллельной обработки
- Lock-free кольцевой буфер
- Высокопроизводительный парсинг и индексация логов

## Быстрый старт

### Требования
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+ (для нативных модулей)

### Настройка среды разработки

```bash
# Клонирование репозитория
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# Запуск инфраструктурных сервисов
docker-compose up -d postgres redis

# Backend
cd backend
pip install -e ".[dev]"
uvicorn src.main:app --reload

# IDE
cd ide
npm install
npm run dev
```

### Развёртывание в продакшене

```bash
# Полный стек через Docker
docker-compose up -d

# Точки доступа
# - API: http://localhost:8000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001
```

## Структура проекта

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE (Electron + Vue 3 + MDUI)
├── phone-and-ipad/
│   ├── ios/               # iOS приложение (SwiftUI)
│   └── android/           # Android приложение (Flutter)
├── backend/               # Python FastAPI Backend
├── native/                # C модули производительности
├── deploy/
│   ├── docker/           # Конфигурации Docker
│   ├── kubernetes/       # K8s манифесты
│   └── bare-metal/       # Скрипты для bare metal
└── docs/                  # Документация
```

## Документация

- [Обзор архитектуры](ARCHITECTURE.md)
- [Справочник API](API.md)
- [Руководство по развёртыванию](DEPLOYMENT.md)
- [Руководство по участию](../CONTRIBUTING.md)

## Варианты развёртывания

| Метод | Описание |
|-------|----------|
| Docker Compose | Однонодовая разработка/тестирование |
| Kubernetes | Продакшен-оркестрация |
| Bare Metal | Прямая установка на Linux/Windows/macOS |
| Облако | AWS, GCP, Azure, Alibaba Cloud |
| Гипервизор | Интеграция с [Zixiao Hypervisor](https://github.com/Zixiao-System/hypervisor) |

## Участие в разработке

Мы приветствуем ваш вклад! Подробности в [Руководстве по участию](../CONTRIBUTING.md).

## Лицензия

Этот проект лицензирован под лицензией MIT — подробности в файле [LICENSE](../LICENSE).

---

<div align="center">

**Создано с любовью командой PRTS**

</div>
