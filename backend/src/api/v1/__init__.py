"""
API v1 router aggregation.
"""

from fastapi import APIRouter

from src.api.v1 import auth, monitoring, pipelines, projects, push

router = APIRouter()

router.include_router(auth.router, prefix="/auth", tags=["Authentication"])
router.include_router(projects.router, prefix="/projects", tags=["Projects"])
router.include_router(pipelines.router, prefix="/pipelines", tags=["Pipelines"])
router.include_router(monitoring.router, prefix="/monitoring", tags=["Monitoring"])
router.include_router(push.router, prefix="/push", tags=["Push Notifications"])
