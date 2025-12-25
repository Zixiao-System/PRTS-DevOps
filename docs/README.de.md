<div align="center">

# PRTS DevOps Plattform

<img src="assets/logo.png" alt="PRTS Logo" width="120" height="120">

**Enterprise-Grade DevOps-Verwaltungsplattform**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### Sprachauswahl

[**English**](../README.md) | [**中文**](README.zh-CN.md) | [**日本語**](README.ja.md) | [**Русский**](README.ru.md) | [**Français**](README.fr.md) | [**Deutsch**](README.de.md)

</div>

---

## Überblick

PRTS (Production Resource Tracking System) ist eine Enterprise-Grade DevOps-Verwaltungsplattform mit folgenden Komponenten:

- **Logos IDE** - Electron-basierte IDE mit Vue 3 + MDUI, Monaco Editor und intelligenter Code-Analyse
- **Mobile Apps** - Native iOS (SwiftUI) und Android (Flutter) Anwendungen mit Push-Benachrichtigungen
- **Backend API** - Python FastAPI mit Async-Unterstützung, Celery Task-Queue und umfassender Überwachung
- **Native Performance-Schicht** - C-Module für hochperformante Metrik-Erfassung und Log-Verarbeitung

> *Der Name „Logos" stammt von einem Elite-Operator im Spiel „Arknights" (明日方舟)*

## Hauptfunktionen

### Logos IDE
- Monaco Editor mit intelligenter Code-Vervollständigung
- Integriertes Terminal (xterm.js)
- CI/CD Pipeline-Verwaltung
- Echtzeit-Überwachungs-Dashboard
- Moderne MDUI-basierte Benutzeroberfläche

### Mobile Anwendungen
- **iOS**: SwiftUI, APNs Push-Benachrichtigungen, Live Activity, Dynamic Island
- **Android**: Flutter, Material 3, Firebase Cloud Messaging

### Backend-Dienste
- RESTful API mit FastAPI
- JWT-Authentifizierung
- PostgreSQL + Redis
- Celery für asynchrone Aufgaben
- Prometheus-Metriken

### Native Performance-Schicht
- Speicherpool zur Reduzierung des Allokations-Overheads
- Thread-Pool für parallele Verarbeitung
- Lock-freier Ringpuffer
- Hochperformanter Log-Parser und Indexer

## Schnellstart

### Voraussetzungen
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+ (für native Module)

### Entwicklungsumgebung einrichten

```bash
# Repository klonen
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# Infrastruktur-Dienste starten
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

### Produktions-Deployment

```bash
# Vollständiger Stack mit Docker
docker-compose up -d

# Zugangspunkte
# - API: http://localhost:8000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001
```

## Projektstruktur

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE (Electron + Vue 3 + MDUI)
├── phone-and-ipad/
│   ├── ios/               # iOS App (SwiftUI)
│   └── android/           # Android App (Flutter)
├── backend/               # Python FastAPI Backend
├── native/                # C Performance-Module
├── deploy/
│   ├── docker/           # Docker-Konfigurationen
│   ├── kubernetes/       # K8s-Manifeste
│   └── bare-metal/       # Bare-Metal-Skripte
└── docs/                  # Dokumentation
```

## Dokumentation

- [Architekturübersicht](ARCHITECTURE.md)
- [API-Referenz](API.md)
- [Deployment-Leitfaden](DEPLOYMENT.md)
- [Beitragsleitfaden](../CONTRIBUTING.md)

## Deployment-Optionen

| Methode | Beschreibung |
|---------|--------------|
| Docker Compose | Einzelknoten-Entwicklung/Test |
| Kubernetes | Produktions-Orchestrierung |
| Bare Metal | Direkte Installation auf Linux/Windows/macOS |
| Cloud | AWS, GCP, Azure, Alibaba Cloud |
| Hypervisor | [Zixiao Hypervisor](https://github.com/Zixiao-System/hypervisor) Integration |

## Mitwirken

Wir begrüßen Beiträge! Details finden Sie in unserem [Beitragsleitfaden](../CONTRIBUTING.md).

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert – siehe die [LICENSE](../LICENSE)-Datei für Details.

---

<div align="center">

**Mit Leidenschaft entwickelt vom PRTS-Team**

</div>
