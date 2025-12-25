<div align="center">

# PRTS DevOps プラットフォーム

<img src="assets/logo.png" alt="PRTS Logo" width="120" height="120">

**エンタープライズグレード DevOps 管理プラットフォーム**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)
[![Python](https://img.shields.io/badge/python-3.11+-blue.svg)](https://python.org)
[![TypeScript](https://img.shields.io/badge/typescript-5.0+-blue.svg)](https://typescriptlang.org)
[![Swift](https://img.shields.io/badge/swift-5.9+-orange.svg)](https://swift.org)
[![Flutter](https://img.shields.io/badge/flutter-3.16+-blue.svg)](https://flutter.dev)

---

### 言語選択

[**English**](../README.md) | [**中文**](README.zh-CN.md) | [**日本語**](README.ja.md) | [**Русский**](README.ru.md) | [**Français**](README.fr.md) | [**Deutsch**](README.de.md)

</div>

---

## 概要

PRTS（Production Resource Tracking System）は、以下のコア機能を備えたエンタープライズグレードの DevOps 管理プラットフォームです：

- **Logos IDE** - Vue 3 + MDUI、Monaco エディタ、インテリジェントなコード分析を備えた Electron ベースの IDE
- **モバイルアプリ** - プッシュ通知対応のネイティブ iOS（SwiftUI）および Android（Flutter）アプリケーション
- **バックエンド API** - 非同期対応の Python FastAPI、Celery タスクキュー、包括的な監視機能
- **ネイティブパフォーマンス層** - 高性能なメトリクス収集とログ処理のための C モジュール

> *「Logos」という名前は、ゲーム「アークナイツ」（明日方舟）のエリートオペレーターに由来しています*

## 主な機能

### Logos IDE
- インテリジェントなコード補完機能を備えた Monaco エディタ
- 統合ターミナル（xterm.js）
- CI/CD パイプライン管理
- リアルタイム監視ダッシュボード
- MDUI ベースのモダンな UI

### モバイルアプリケーション
- **iOS**：SwiftUI、APNs プッシュ通知、ライブアクティビティ、ダイナミックアイランド
- **Android**：Flutter、Material 3、Firebase Cloud Messaging

### バックエンドサービス
- FastAPI による RESTful API
- JWT 認証
- PostgreSQL + Redis
- Celery による非同期タスク処理
- Prometheus メトリクス

### ネイティブパフォーマンス層
- メモリプール（アロケーションオーバーヘッド削減）
- スレッドプール（並列処理）
- ロックフリーリングバッファ
- 高性能ログパーサーとインデクサー

## クイックスタート

### 前提条件
- Docker & Docker Compose
- Node.js 18+
- Python 3.11+
- CMake 3.20+（ネイティブモジュール用）

### 開発環境のセットアップ

```bash
# リポジトリをクローン
git clone https://github.com/your-org/PRTS_DevOps_platform.git
cd PRTS_DevOps_platform

# インフラサービスを起動
docker-compose up -d postgres redis

# バックエンド
cd backend
pip install -e ".[dev]"
uvicorn src.main:app --reload

# IDE
cd ide
npm install
npm run dev
```

### 本番環境へのデプロイ

```bash
# Docker でフルスタックをデプロイ
docker-compose up -d

# アクセスポイント
# - API: http://localhost:8000
# - Prometheus: http://localhost:9090
# - Grafana: http://localhost:3001
```

## プロジェクト構成

```
PRTS_DevOps_platform/
├── ide/                    # Logos IDE（Electron + Vue 3 + MDUI）
├── phone-and-ipad/
│   ├── ios/               # iOS アプリ（SwiftUI）
│   └── android/           # Android アプリ（Flutter）
├── backend/               # Python FastAPI バックエンド
├── native/                # C パフォーマンスモジュール
├── deploy/
│   ├── docker/           # Docker 設定
│   ├── kubernetes/       # K8s マニフェスト
│   └── bare-metal/       # ベアメタルスクリプト
└── docs/                  # ドキュメント
```

## ドキュメント

- [アーキテクチャ概要](ARCHITECTURE.md)
- [API リファレンス](API.md)
- [デプロイガイド](DEPLOYMENT.md)
- [コントリビューションガイド](../CONTRIBUTING.md)

## デプロイオプション

| 方法 | 説明 |
|------|------|
| Docker Compose | シングルノード開発/テスト |
| Kubernetes | 本番グレードのオーケストレーション |
| ベアメタル | Linux/Windows/macOS 直接インストール |
| クラウド | AWS、GCP、Azure、Alibaba Cloud |
| ハイパーバイザー | [紫霄 Hypervisor](https://github.com/Zixiao-System/hypervisor) 統合 |

## コントリビューション

コントリビューションを歓迎します！詳細は[コントリビューションガイド](../CONTRIBUTING.md)をご覧ください。

## ライセンス

このプロジェクトは MIT ライセンスの下で公開されています - 詳細は [LICENSE](../LICENSE) ファイルをご覧ください。

---

<div align="center">

**PRTS チームが情熱を込めて開発**

</div>
