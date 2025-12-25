<div align="center">

# PRTS DevOps Platform

<img src="docs/assets/logo.png" alt="PRTS Logo" width="120" height="120">

**Enterprise-grade DevOps Management Platform**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### Language / 语言 / 言語 / Язык / Langue / Sprache

[**English**](README.md) | [**中文**](docs/README.zh-CN.md) | [**日本語**](docs/README.ja.md) | [**Русский**](docs/README.ru.md) | [**Français**](docs/README.fr.md) | [**Deutsch**](docs/README.de.md)

</div>

---

## Overview

PRTS (Production Resource Tracking System) is an enterprise-grade DevOps management platform featuring:

- **Logos IDE** - Electron-based IDE with Vue 3 + MDUI, Monaco Editor, and intelligent code analysis
- **Mobile Apps** - Native iOS (SwiftUI) and Android (Flutter) applications with push notifications
- **Backend API** - Python FastAPI with async support, Celery task queue, and comprehensive monitoring
- **Native Performance** - C modules for high-performance metrics collection and log processing

> *The name "Logos" comes from an elite operator in the game "Arknights" (明日方舟)*

## Features

### Logos IDE

> **Note**: Logos IDE is being migrated to a standalone repository for independent development and releases. See [ide/MIGRATION_PLAN.md](ide/MIGRATION_PLAN.md) for details.

- Monaco Editor with intelligent code completion
- Integrated terminal (xterm.js)
- Single-click file opening (VS Code style)
- Git integration and branch management
- CI/CD pipeline management
- Real-time monitoring dashboard
- MDUI-based modern Material Design UI
- Dark/Light theme support

For more information, see the [Logos IDE README](ide/README.md).

### Mobile Applications
- **iOS**: SwiftUI, APNs push notifications, Live Activity, Dynamic Island
- **Android**: Flutter, Material 3, Firebase Cloud Messaging

### Backend Services
- RESTful API with FastAPI
- JWT authentication
- PostgreSQL + Redis
- Celery for async tasks
- Prometheus metrics

### Native Performance Layer
- Memory pool for reduced allocation overhead
- Thread pool for parallel processing
- Lock-free ring buffer
- High-performance log parsing and indexing

## Quick Start

### Prerequisites
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+ (for native modules)

### Development Setup

```bash
# Clone the repository
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# Start infrastructure services
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

### Production Deployment

```bash
# Full stack with Docker
docker-compose up -d

# Access points
# - API: http://localhost:8000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001
```

## Project Structure

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE (Electron + Vue 3 + MDUI)
│   ├── README.md          # IDE documentation
│   └── MIGRATION_PLAN.md  # Standalone repo migration plan
├── phone-and-ipad/
│   ├── ios/               # iOS App (SwiftUI)
│   └── android/           # Android App (Flutter)
├── backend/               # Python FastAPI Backend
├── native/                # C Performance Modules
├── deploy/
│   ├── docker/           # Docker configurations
│   ├── kubernetes/       # K8s manifests
│   └── bare-metal/       # Bare metal scripts
└── docs/                  # Documentation
```

## Documentation

- [Architecture Overview](docs/ARCHITECTURE.md)
- [API Reference](docs/API.md)
- [Deployment Guide](docs/DEPLOYMENT.md)
- [Contributing Guide](CONTRIBUTING.md)

## Deployment Options

| Method | Description |
|--------|-------------|
| Docker Compose | Single-node development/testing |
| Kubernetes | Production-grade orchestration |
| Bare Metal | Linux/Windows/macOS direct installation |
| Cloud | AWS, GCP, Azure, Alibaba Cloud |
| Hypervisor | [Zixiao Hypervisor](https://github.com/Zixiao-System/hypervisor) integration |

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**Built with passion by the PRTS Team**

</div>
