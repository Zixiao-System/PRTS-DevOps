<div align="center">

# Plateforme PRTS DevOps

<img src="assets/logo.png" alt="PRTS Logo" width="120" height="120">

**Plateforme de gestion DevOps de niveau entreprise**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### Sélection de la langue

[**English**](../README.md) | [**中文**](README.zh-CN.md) | [**日本語**](README.ja.md) | [**Русский**](README.ru.md) | [**Français**](README.fr.md) | [**Deutsch**](README.de.md)

</div>

---

## Aperçu

PRTS (Production Resource Tracking System) est une plateforme de gestion DevOps de niveau entreprise comprenant :

- **Logos IDE** - IDE basé sur Electron avec Vue 3 + MDUI, éditeur Monaco et analyse de code intelligente
- **Applications mobiles** - Applications natives iOS (SwiftUI) et Android (Flutter) avec notifications push
- **API Backend** - Python FastAPI asynchrone, file de tâches Celery et surveillance complète
- **Couche de performance native** - Modules C pour la collecte de métriques et le traitement des logs haute performance

> *Le nom « Logos » provient d'un opérateur d'élite du jeu « Arknights » (明日方舟)*

## Fonctionnalités principales

### Logos IDE
- Éditeur Monaco avec complétion de code intelligente
- Terminal intégré (xterm.js)
- Gestion des pipelines CI/CD
- Tableau de bord de surveillance en temps réel
- Interface moderne basée sur MDUI

### Applications mobiles
- **iOS** : SwiftUI, notifications push APNs, Live Activity, Dynamic Island
- **Android** : Flutter, Material 3, Firebase Cloud Messaging

### Services Backend
- API RESTful avec FastAPI
- Authentification JWT
- PostgreSQL + Redis
- Tâches asynchrones avec Celery
- Métriques Prometheus

### Couche de performance native
- Pool de mémoire pour réduire les surcharges d'allocation
- Pool de threads pour le traitement parallèle
- Buffer circulaire sans verrou
- Analyse et indexation de logs haute performance

## Démarrage rapide

### Prérequis
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+ (pour les modules natifs)

### Configuration de l'environnement de développement

```bash
# Cloner le dépôt
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# Démarrer les services d'infrastructure
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

### Déploiement en production

```bash
# Stack complète avec Docker
docker-compose up -d

# Points d'accès
# - API : http://localhost:8000
# - Prometheus : http://localhost:9090
# - Grafana : http://localhost:3001
```

## Structure du projet

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE (Electron + Vue 3 + MDUI)
├── phone-and-ipad/
│   ├── ios/               # Application iOS (SwiftUI)
│   └── android/           # Application Android (Flutter)
├── backend/               # Backend Python FastAPI
├── native/                # Modules de performance C
├── deploy/
│   ├── docker/           # Configurations Docker
│   ├── kubernetes/       # Manifestes K8s
│   └── bare-metal/       # Scripts bare metal
└── docs/                  # Documentation
```

## Documentation

- [Aperçu de l'architecture](ARCHITECTURE.md)
- [Référence API](API.md)
- [Guide de déploiement](DEPLOYMENT.md)
- [Guide de contribution](../CONTRIBUTING.md)

## Options de déploiement

| Méthode | Description |
|---------|-------------|
| Docker Compose | Développement/test sur un seul nœud |
| Kubernetes | Orchestration de niveau production |
| Bare Metal | Installation directe sur Linux/Windows/macOS |
| Cloud | AWS, GCP, Azure, Alibaba Cloud |
| Hyperviseur | Intégration [Zixiao Hypervisor](https://github.com/Zixiao-System/hypervisor) |

## Contribution

Nous accueillons les contributions ! Consultez notre [Guide de contribution](../CONTRIBUTING.md) pour plus de détails.

## Licence

Ce projet est sous licence MIT - voir le fichier [LICENSE](../LICENSE) pour plus de détails.

---

<div align="center">

**Construit avec passion par l'équipe PRTS**

</div>
