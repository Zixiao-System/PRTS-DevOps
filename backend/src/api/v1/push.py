"""
Push Notifications API endpoints.
Supports APNs (iOS) and FCM (Android).
"""

from datetime import datetime
from typing import Annotated, List, Optional

from fastapi import APIRouter, Depends, HTTPException, status
from pydantic import BaseModel

from src.api.v1.auth import get_current_user

router = APIRouter()


# Models
class DeviceRegistration(BaseModel):
    """Device registration request."""

    device_token: str
    platform: str  # "ios" or "android"
    device_name: Optional[str] = None
    app_version: Optional[str] = None


class DeviceResponse(BaseModel):
    """Device response model."""

    id: str
    user_id: str
    device_token: str
    platform: str
    device_name: Optional[str]
    app_version: Optional[str]
    registered_at: datetime
    last_active: datetime


class PushNotificationRequest(BaseModel):
    """Push notification request."""

    title: str
    body: str
    data: Optional[dict] = None
    badge: Optional[int] = None
    sound: Optional[str] = "default"
    user_ids: Optional[List[str]] = None  # None means all users
    topic: Optional[str] = None  # For topic-based notifications


class PushNotificationResponse(BaseModel):
    """Push notification response."""

    id: str
    status: str
    sent_count: int
    failed_count: int
    created_at: datetime


class NotificationPreferences(BaseModel):
    """User notification preferences."""

    push_enabled: bool = True
    critical_alerts: bool = True
    warning_alerts: bool = True
    info_alerts: bool = False
    build_notifications: bool = True
    deploy_notifications: bool = True
    sound_enabled: bool = True
    vibration_enabled: bool = True


# Mock data
MOCK_DEVICES: List[dict] = []
MOCK_NOTIFICATIONS: List[dict] = []
MOCK_PREFERENCES: dict[str, dict] = {}


@router.post("/devices/register", response_model=DeviceResponse, status_code=status.HTTP_201_CREATED)
async def register_device(
    device_data: DeviceRegistration,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Register a device for push notifications."""
    # Check if device already registered
    existing = next(
        (d for d in MOCK_DEVICES if d["device_token"] == device_data.device_token),
        None,
    )

    if existing:
        # Update existing device
        existing["user_id"] = current_user["id"]
        existing["platform"] = device_data.platform
        existing["device_name"] = device_data.device_name
        existing["app_version"] = device_data.app_version
        existing["last_active"] = datetime.now()
        return existing

    # Create new device registration
    new_device = {
        "id": str(len(MOCK_DEVICES) + 1),
        "user_id": current_user["id"],
        "device_token": device_data.device_token,
        "platform": device_data.platform,
        "device_name": device_data.device_name,
        "app_version": device_data.app_version,
        "registered_at": datetime.now(),
        "last_active": datetime.now(),
    }
    MOCK_DEVICES.append(new_device)

    return new_device


@router.delete("/devices/{device_token}", status_code=status.HTTP_204_NO_CONTENT)
async def unregister_device(
    device_token: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> None:
    """Unregister a device from push notifications."""
    global MOCK_DEVICES
    device = next(
        (d for d in MOCK_DEVICES if d["device_token"] == device_token),
        None,
    )

    if not device:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Device not found",
        )

    if device["user_id"] != current_user["id"]:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not authorized to unregister this device",
        )

    MOCK_DEVICES = [d for d in MOCK_DEVICES if d["device_token"] != device_token]


@router.get("/devices", response_model=List[DeviceResponse])
async def list_devices(
    current_user: Annotated[dict, Depends(get_current_user)],
) -> List[dict]:
    """List all registered devices for the current user."""
    return [d for d in MOCK_DEVICES if d["user_id"] == current_user["id"]]


@router.post("/send", response_model=PushNotificationResponse)
async def send_notification(
    notification: PushNotificationRequest,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Send a push notification (admin only)."""
    # In a real implementation, check admin permissions

    # Get target devices
    if notification.user_ids:
        target_devices = [
            d for d in MOCK_DEVICES if d["user_id"] in notification.user_ids
        ]
    else:
        target_devices = MOCK_DEVICES

    # Separate by platform
    ios_devices = [d for d in target_devices if d["platform"] == "ios"]
    android_devices = [d for d in target_devices if d["platform"] == "android"]

    # Mock sending
    sent_count = len(target_devices)
    failed_count = 0

    # In a real implementation:
    # - Use aioapns for iOS
    # - Use firebase-admin for Android

    notification_record = {
        "id": str(len(MOCK_NOTIFICATIONS) + 1),
        "title": notification.title,
        "body": notification.body,
        "data": notification.data,
        "status": "sent",
        "sent_count": sent_count,
        "failed_count": failed_count,
        "ios_count": len(ios_devices),
        "android_count": len(android_devices),
        "created_at": datetime.now(),
    }
    MOCK_NOTIFICATIONS.append(notification_record)

    return notification_record


@router.get("/preferences", response_model=NotificationPreferences)
async def get_preferences(
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Get notification preferences for the current user."""
    user_id = current_user["id"]
    if user_id not in MOCK_PREFERENCES:
        MOCK_PREFERENCES[user_id] = NotificationPreferences().model_dump()
    return MOCK_PREFERENCES[user_id]


@router.put("/preferences", response_model=NotificationPreferences)
async def update_preferences(
    preferences: NotificationPreferences,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Update notification preferences for the current user."""
    user_id = current_user["id"]
    MOCK_PREFERENCES[user_id] = preferences.model_dump()
    return MOCK_PREFERENCES[user_id]


# APNs specific endpoints
@router.post("/apns/send")
async def send_apns_notification(
    device_tokens: List[str],
    title: str,
    body: str,
    badge: Optional[int] = None,
    current_user: Annotated[dict, Depends(get_current_user)] = None,
) -> dict:
    """
    Send APNs notification to iOS devices.

    In production, this would use aioapns:
    ```python
    from aioapns import APNs, NotificationRequest, PushType

    apns = APNs(
        key=settings.APNS_KEY_PATH,
        key_id=settings.APNS_KEY_ID,
        team_id=settings.APNS_TEAM_ID,
        topic=settings.APNS_BUNDLE_ID,
        use_sandbox=settings.APNS_USE_SANDBOX,
    )

    for token in device_tokens:
        request = NotificationRequest(
            device_token=token,
            message={
                "aps": {
                    "alert": {"title": title, "body": body},
                    "badge": badge,
                    "sound": "default",
                }
            },
            push_type=PushType.ALERT,
        )
        response = await apns.send_notification(request)
    ```
    """
    return {
        "status": "sent",
        "platform": "apns",
        "device_count": len(device_tokens),
    }


# FCM specific endpoints
@router.post("/fcm/send")
async def send_fcm_notification(
    device_tokens: List[str],
    title: str,
    body: str,
    data: Optional[dict] = None,
    current_user: Annotated[dict, Depends(get_current_user)] = None,
) -> dict:
    """
    Send FCM notification to Android devices.

    In production, this would use firebase-admin:
    ```python
    import firebase_admin
    from firebase_admin import credentials, messaging

    cred = credentials.Certificate(settings.FIREBASE_CREDENTIALS_PATH)
    firebase_admin.initialize_app(cred)

    message = messaging.MulticastMessage(
        notification=messaging.Notification(
            title=title,
            body=body,
        ),
        data=data or {},
        tokens=device_tokens,
    )
    response = messaging.send_multicast(message)
    ```
    """
    return {
        "status": "sent",
        "platform": "fcm",
        "device_count": len(device_tokens),
    }


# Live Activity (iOS) endpoints
@router.post("/live-activity/start")
async def start_live_activity(
    activity_type: str,
    data: dict,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """
    Start a Live Activity on iOS.

    This would trigger a push notification that starts a Live Activity
    for pipeline builds, deployments, etc.
    """
    return {
        "status": "started",
        "activity_type": activity_type,
        "activity_id": "live-activity-123",
    }


@router.post("/live-activity/{activity_id}/update")
async def update_live_activity(
    activity_id: str,
    data: dict,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Update an existing Live Activity."""
    return {
        "status": "updated",
        "activity_id": activity_id,
    }


@router.post("/live-activity/{activity_id}/end")
async def end_live_activity(
    activity_id: str,
    final_data: dict,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """End a Live Activity."""
    return {
        "status": "ended",
        "activity_id": activity_id,
    }
