<div align="center">

# PRTS DevOps 平台

<img src="assets/logo.png" alt="PRTS Logo" width="120" height="120">

**企业级 DevOps 管理平台**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### 语言选择

[**English**](../README.md) | [**中文**](README.zh-CN.md) | [**日本語**](README.ja.md) | [**Русский**](README.ru.md) | [**Français**](README.fr.md) | [**Deutsch**](README.de.md)

</div>

---

## 概述

PRTS（生产资源追踪系统）是一个企业级 DevOps 管理平台，包含以下核心组件：

- **Logos IDE** - 基于 Electron 的集成开发环境，采用 Vue 3 + MDUI、Monaco 编辑器和智能代码分析
- **移动应用** - 原生 iOS（SwiftUI）和 Android（Flutter）应用，支持推送通知
- **后端 API** - Python FastAPI 异步后端，Celery 任务队列，完善的监控体系
- **原生性能层** - 用于高性能指标采集和日志处理的 C 语言模块

> *"Logos"这个名字来源于游戏《明日方舟》中的一名精英干员*

## 核心功能

### Logos IDE
- Monaco 编辑器，智能代码补全
- 集成终端（xterm.js）
- CI/CD 流水线管理
- 实时监控仪表盘
- 基于 MDUI 的现代化界面

### 移动应用
- **iOS**：SwiftUI、APNs 推送、实时活动（Live Activity）、灵动岛
- **Android**：Flutter、Material 3、Firebase 云消息推送

### 后端服务
- 基于 FastAPI 的 RESTful API
- JWT 身份认证
- PostgreSQL + Redis 数据存储
- Celery 异步任务队列
- Prometheus 指标监控

### 原生性能层
- 内存池，减少内存分配开销
- 线程池，并行任务处理
- 无锁环形缓冲区
- 高性能日志解析和索引

## 快速开始

### 环境要求
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+（编译原生模块）

### 开发环境搭建

```bash
# 克隆仓库
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# 启动基础设施服务
docker-compose up -d postgres redis

# 后端
cd backend
pip install -e ".[dev]"
uvicorn src.main:app --reload

# IDE
cd ide
npm install
npm run dev
```

### 生产环境部署

```bash
# 使用 Docker 部署全栈
docker-compose up -d

# 访问地址
# - API: http://localhost:8000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001
```

## 项目结构

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE（Electron + Vue 3 + MDUI）
├── phone-and-ipad/
│   ├── ios/               # iOS 应用（SwiftUI）
│   └── android/           # Android 应用（Flutter）
├── backend/               # Python FastAPI 后端
├── native/                # C 语言性能模块
├── deploy/
│   ├── docker/           # Docker 配置
│   ├── kubernetes/       # K8s 部署清单
│   └── bare-metal/       # 裸机部署脚本
└── docs/                  # 文档
```

## 文档

- [架构概览](ARCHITECTURE.md)
- [API 参考](API.md)
- [部署指南](DEPLOYMENT.md)
- [贡献指南](../CONTRIBUTING.md)

## 部署方式

| 方式 | 说明 |
|------|------|
| Docker Compose | 单节点开发/测试环境 |
| Kubernetes | 生产级容器编排 |
| 裸机部署 | Linux/Windows/macOS 直接安装 |
| 公有云 | AWS、GCP、Azure、阿里云 |
| 虚拟化平台 | [紫霄 Hypervisor](https://github.com/Zixiao-System/hypervisor) 集成 |

## 贡献

欢迎贡献代码！请查阅 [贡献指南](../CONTRIBUTING.md) 了解详情。

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](../LICENSE) 文件。

---

<div align="center">

**由 Zixiao-System 倾心打造**

</div>
