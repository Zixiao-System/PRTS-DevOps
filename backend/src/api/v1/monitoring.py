"""
Monitoring API endpoints.
"""

import random
from datetime import datetime, timedelta
from typing import Annotated, List, Optional

from fastapi import APIRouter, Depends, Query
from pydantic import BaseModel

from src.api.v1.auth import get_current_user

router = APIRouter()


# Models
class MetricPoint(BaseModel):
    """Single metric data point."""

    timestamp: datetime
    value: float


class MetricsResponse(BaseModel):
    """Metrics response model."""

    name: str
    unit: str
    current: float
    change: float
    change_percent: float
    history: List[MetricPoint]


class AlertResponse(BaseModel):
    """Alert response model."""

    id: str
    title: str
    message: str
    severity: str  # critical, warning, info
    source: str
    category: str
    timestamp: datetime
    is_read: bool
    is_acknowledged: bool


class ServerStatusResponse(BaseModel):
    """Server status response model."""

    id: str
    name: str
    ip_address: str
    status: str  # online, offline, warning
    cpu_usage: float
    memory_usage: float
    disk_usage: float
    uptime_seconds: int


# Mock data generators
def generate_metric_history(base_value: float, variance: float, hours: int = 24) -> List[dict]:
    """Generate mock metric history data."""
    return [
        {
            "timestamp": datetime.now() - timedelta(hours=hours - i),
            "value": base_value + random.uniform(-variance, variance),
        }
        for i in range(hours)
    ]


MOCK_ALERTS: List[dict] = [
    {
        "id": "1",
        "title": "CPU 使用率超过 90%",
        "message": "服务器 prod-server-01 的 CPU 使用率达到 92%，请及时处理",
        "severity": "critical",
        "source": "prod-server-01",
        "category": "性能监控",
        "timestamp": datetime.now() - timedelta(minutes=5),
        "is_read": False,
        "is_acknowledged": False,
    },
    {
        "id": "2",
        "title": "构建失败",
        "message": "frontend-deploy 流水线构建失败，错误: npm install timeout",
        "severity": "critical",
        "source": "CI/CD",
        "category": "构建告警",
        "timestamp": datetime.now() - timedelta(minutes=15),
        "is_read": False,
        "is_acknowledged": False,
    },
    {
        "id": "3",
        "title": "磁盘空间不足",
        "message": "服务器 db-server-01 的磁盘使用率达到 85%",
        "severity": "warning",
        "source": "db-server-01",
        "category": "资源监控",
        "timestamp": datetime.now() - timedelta(hours=1),
        "is_read": True,
        "is_acknowledged": False,
    },
    {
        "id": "4",
        "title": "SSL 证书即将过期",
        "message": "api.example.com 的 SSL 证书将在 7 天后过期",
        "severity": "warning",
        "source": "安全检查",
        "category": "安全告警",
        "timestamp": datetime.now() - timedelta(hours=3),
        "is_read": True,
        "is_acknowledged": True,
    },
    {
        "id": "5",
        "title": "部署完成",
        "message": "backend-api v2.3.1 已成功部署到生产环境",
        "severity": "info",
        "source": "CI/CD",
        "category": "部署通知",
        "timestamp": datetime.now() - timedelta(hours=5),
        "is_read": True,
        "is_acknowledged": True,
    },
]

MOCK_SERVERS: List[dict] = [
    {
        "id": "1",
        "name": "prod-server-01",
        "ip_address": "10.0.1.10",
        "status": "warning",
        "cpu_usage": 92.3,
        "memory_usage": 68.5,
        "disk_usage": 45.2,
        "uptime_seconds": 2592000,
    },
    {
        "id": "2",
        "name": "prod-server-02",
        "ip_address": "10.0.1.11",
        "status": "online",
        "cpu_usage": 45.7,
        "memory_usage": 52.3,
        "disk_usage": 38.9,
        "uptime_seconds": 1728000,
    },
    {
        "id": "3",
        "name": "db-server-01",
        "ip_address": "10.0.2.10",
        "status": "warning",
        "cpu_usage": 35.2,
        "memory_usage": 78.6,
        "disk_usage": 85.1,
        "uptime_seconds": 5184000,
    },
    {
        "id": "4",
        "name": "staging-server",
        "ip_address": "10.0.3.10",
        "status": "online",
        "cpu_usage": 12.4,
        "memory_usage": 34.5,
        "disk_usage": 22.7,
        "uptime_seconds": 864000,
    },
]


@router.get("/metrics", response_model=dict)
async def get_metrics(
    current_user: Annotated[dict, Depends(get_current_user)],
    time_range: str = Query("24h", description="Time range: 1h, 6h, 24h, 7d, 30d"),
) -> dict:
    """Get system metrics overview."""
    hours = {"1h": 1, "6h": 6, "24h": 24, "7d": 168, "30d": 720}.get(time_range, 24)

    return {
        "cpu": {
            "name": "CPU 使用率",
            "unit": "%",
            "current": 42.5,
            "change": 5.2,
            "change_percent": 13.9,
            "history": generate_metric_history(42.5, 15, hours),
        },
        "memory": {
            "name": "内存使用",
            "unit": "GB",
            "current": 6.2,
            "change": -0.3,
            "change_percent": -4.6,
            "history": generate_metric_history(6.2, 1.5, hours),
        },
        "disk": {
            "name": "磁盘使用",
            "unit": "%",
            "current": 58.3,
            "change": 2.1,
            "change_percent": 3.7,
            "history": generate_metric_history(58.3, 5, hours),
        },
        "network": {
            "name": "网络流量",
            "unit": "MB/s",
            "current": 125.6,
            "change": 15.3,
            "change_percent": 13.9,
            "history": generate_metric_history(125.6, 50, hours),
        },
        "build_success_rate": {
            "name": "构建成功率",
            "unit": "%",
            "current": 94.2,
            "change": 2.1,
            "change_percent": 2.3,
            "history": generate_metric_history(94.2, 5, hours),
        },
        "active_services": {
            "name": "活跃服务",
            "unit": "",
            "current": 12,
            "change": 0,
            "change_percent": 0,
            "history": [],
        },
    }


@router.get("/alerts", response_model=List[AlertResponse])
async def list_alerts(
    current_user: Annotated[dict, Depends(get_current_user)],
    severity: Optional[str] = Query(None, description="Filter by severity"),
    unread_only: bool = Query(False, description="Show only unread alerts"),
    skip: int = Query(0, ge=0),
    limit: int = Query(20, ge=1, le=100),
) -> List[dict]:
    """List alerts with optional filtering."""
    alerts = MOCK_ALERTS

    if severity:
        alerts = [a for a in alerts if a["severity"] == severity]

    if unread_only:
        alerts = [a for a in alerts if not a["is_read"]]

    # Sort by timestamp descending
    alerts = sorted(alerts, key=lambda x: x["timestamp"], reverse=True)

    return alerts[skip : skip + limit]


@router.post("/alerts/{alert_id}/acknowledge")
async def acknowledge_alert(
    alert_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Acknowledge an alert."""
    alert = next((a for a in MOCK_ALERTS if a["id"] == alert_id), None)
    if not alert:
        return {"error": "Alert not found"}

    alert["is_acknowledged"] = True
    alert["is_read"] = True

    return {"message": "Alert acknowledged", "alert_id": alert_id}


@router.post("/alerts/{alert_id}/read")
async def mark_alert_read(
    alert_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Mark an alert as read."""
    alert = next((a for a in MOCK_ALERTS if a["id"] == alert_id), None)
    if not alert:
        return {"error": "Alert not found"}

    alert["is_read"] = True

    return {"message": "Alert marked as read", "alert_id": alert_id}


@router.post("/alerts/read-all")
async def mark_all_alerts_read(
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Mark all alerts as read."""
    for alert in MOCK_ALERTS:
        alert["is_read"] = True

    return {"message": "All alerts marked as read"}


@router.get("/servers", response_model=List[ServerStatusResponse])
async def list_servers(
    current_user: Annotated[dict, Depends(get_current_user)],
    status: Optional[str] = Query(None, description="Filter by status"),
) -> List[dict]:
    """List all monitored servers."""
    servers = MOCK_SERVERS

    if status:
        servers = [s for s in servers if s["status"] == status]

    return servers


@router.get("/servers/{server_id}", response_model=ServerStatusResponse)
async def get_server(
    server_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Get detailed status for a specific server."""
    server = next((s for s in MOCK_SERVERS if s["id"] == server_id), None)
    if not server:
        return {"error": "Server not found"}

    return server


@router.get("/servers/{server_id}/metrics")
async def get_server_metrics(
    server_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
    time_range: str = Query("24h", description="Time range"),
) -> dict:
    """Get detailed metrics for a specific server."""
    server = next((s for s in MOCK_SERVERS if s["id"] == server_id), None)
    if not server:
        return {"error": "Server not found"}

    hours = {"1h": 1, "6h": 6, "24h": 24, "7d": 168, "30d": 720}.get(time_range, 24)

    return {
        "server_id": server_id,
        "server_name": server["name"],
        "cpu": generate_metric_history(server["cpu_usage"], 20, hours),
        "memory": generate_metric_history(server["memory_usage"], 15, hours),
        "disk": generate_metric_history(server["disk_usage"], 5, hours),
        "network_in": generate_metric_history(50, 30, hours),
        "network_out": generate_metric_history(30, 20, hours),
    }
