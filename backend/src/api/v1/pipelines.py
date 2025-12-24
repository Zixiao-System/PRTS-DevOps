"""
Pipelines API endpoints.
"""

from datetime import datetime, timedelta
from typing import Annotated, List, Optional

from fastapi import APIRouter, Depends, HTTPException, Query, status
from pydantic import BaseModel

from src.api.v1.auth import get_current_user

router = APIRouter()


# Enums and Models
class PipelineStatus:
    PENDING = "pending"
    RUNNING = "running"
    SUCCESS = "success"
    FAILED = "failed"
    CANCELLED = "cancelled"


class PipelineTrigger(BaseModel):
    """Pipeline trigger request."""

    project_id: str
    branch: str = "main"
    variables: Optional[dict] = None


class PipelineResponse(BaseModel):
    """Pipeline response model."""

    id: str
    name: str
    project_id: str
    project_name: str
    branch: str
    status: str
    triggered_by: str
    started_at: datetime
    finished_at: Optional[datetime]
    duration_seconds: int
    stages: List[str]
    current_stage: int
    error_message: Optional[str]


class PipelineStageResponse(BaseModel):
    """Pipeline stage response model."""

    name: str
    status: str
    started_at: Optional[datetime]
    finished_at: Optional[datetime]
    duration_seconds: int
    logs: Optional[str]


# Mock data
MOCK_PIPELINES: List[dict] = [
    {
        "id": "1",
        "name": "frontend-deploy",
        "project_id": "1",
        "project_name": "PRTS Web",
        "branch": "main",
        "status": PipelineStatus.SUCCESS,
        "triggered_by": "张三",
        "started_at": datetime.now() - timedelta(hours=2),
        "finished_at": datetime.now() - timedelta(hours=1, minutes=56),
        "duration_seconds": 222,
        "stages": ["Build", "Test", "Deploy"],
        "current_stage": 2,
        "error_message": None,
    },
    {
        "id": "2",
        "name": "backend-build",
        "project_id": "2",
        "project_name": "PRTS API",
        "branch": "develop",
        "status": PipelineStatus.RUNNING,
        "triggered_by": "李四",
        "started_at": datetime.now() - timedelta(minutes=5),
        "finished_at": None,
        "duration_seconds": 80,
        "stages": ["Build", "Test", "Deploy"],
        "current_stage": 1,
        "error_message": None,
    },
    {
        "id": "3",
        "name": "integration-test",
        "project_id": "3",
        "project_name": "PRTS Core",
        "branch": "feature/auth",
        "status": PipelineStatus.FAILED,
        "triggered_by": "王五",
        "started_at": datetime.now() - timedelta(hours=1),
        "finished_at": datetime.now() - timedelta(minutes=50),
        "duration_seconds": 310,
        "stages": ["Build", "Test", "Deploy"],
        "current_stage": 1,
        "error_message": "Test case failed: auth_integration_test.go:42",
    },
    {
        "id": "4",
        "name": "nightly-build",
        "project_id": "1",
        "project_name": "PRTS Web",
        "branch": "main",
        "status": PipelineStatus.PENDING,
        "triggered_by": "系统",
        "started_at": datetime.now() + timedelta(hours=2),
        "finished_at": None,
        "duration_seconds": 0,
        "stages": ["Build", "Test", "Deploy"],
        "current_stage": 0,
        "error_message": None,
    },
]


@router.get("", response_model=List[PipelineResponse])
async def list_pipelines(
    current_user: Annotated[dict, Depends(get_current_user)],
    project_id: Optional[str] = Query(None, description="Filter by project ID"),
    status: Optional[str] = Query(None, description="Filter by status"),
    skip: int = Query(0, ge=0),
    limit: int = Query(20, ge=1, le=100),
) -> List[dict]:
    """List all pipelines with optional filtering."""
    pipelines = MOCK_PIPELINES

    if project_id:
        pipelines = [p for p in pipelines if p["project_id"] == project_id]

    if status:
        pipelines = [p for p in pipelines if p["status"] == status]

    # Sort by started_at descending
    pipelines = sorted(pipelines, key=lambda x: x["started_at"], reverse=True)

    return pipelines[skip : skip + limit]


@router.post("/trigger", response_model=PipelineResponse, status_code=status.HTTP_201_CREATED)
async def trigger_pipeline(
    trigger_data: PipelineTrigger,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Trigger a new pipeline run."""
    new_pipeline = {
        "id": str(len(MOCK_PIPELINES) + 1),
        "name": f"pipeline-{len(MOCK_PIPELINES) + 1}",
        "project_id": trigger_data.project_id,
        "project_name": "PRTS Web",  # Would be fetched from project
        "branch": trigger_data.branch,
        "status": PipelineStatus.PENDING,
        "triggered_by": current_user["full_name"],
        "started_at": datetime.now(),
        "finished_at": None,
        "duration_seconds": 0,
        "stages": ["Build", "Test", "Deploy"],
        "current_stage": 0,
        "error_message": None,
    }
    MOCK_PIPELINES.append(new_pipeline)

    return new_pipeline


@router.get("/{pipeline_id}", response_model=PipelineResponse)
async def get_pipeline(
    pipeline_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Get a specific pipeline by ID."""
    pipeline = next((p for p in MOCK_PIPELINES if p["id"] == pipeline_id), None)
    if not pipeline:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Pipeline not found",
        )
    return pipeline


@router.get("/{pipeline_id}/stages", response_model=List[PipelineStageResponse])
async def get_pipeline_stages(
    pipeline_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> List[dict]:
    """Get stages for a specific pipeline."""
    pipeline = next((p for p in MOCK_PIPELINES if p["id"] == pipeline_id), None)
    if not pipeline:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Pipeline not found",
        )

    stages = []
    for i, stage_name in enumerate(pipeline["stages"]):
        if i < pipeline["current_stage"]:
            stage_status = "success"
        elif i == pipeline["current_stage"]:
            if pipeline["status"] == PipelineStatus.FAILED:
                stage_status = "failed"
            elif pipeline["status"] == PipelineStatus.RUNNING:
                stage_status = "running"
            else:
                stage_status = "pending"
        else:
            stage_status = "pending"

        stages.append({
            "name": stage_name,
            "status": stage_status,
            "started_at": pipeline["started_at"] if i <= pipeline["current_stage"] else None,
            "finished_at": pipeline["finished_at"] if i < pipeline["current_stage"] else None,
            "duration_seconds": 83 if i < pipeline["current_stage"] else 0,
            "logs": f"Building stage {stage_name}..." if stage_status != "pending" else None,
        })

    return stages


@router.post("/{pipeline_id}/cancel", response_model=PipelineResponse)
async def cancel_pipeline(
    pipeline_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Cancel a running pipeline."""
    pipeline = next((p for p in MOCK_PIPELINES if p["id"] == pipeline_id), None)
    if not pipeline:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Pipeline not found",
        )

    if pipeline["status"] not in [PipelineStatus.RUNNING, PipelineStatus.PENDING]:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Pipeline is not running",
        )

    pipeline["status"] = PipelineStatus.CANCELLED
    pipeline["finished_at"] = datetime.now()

    return pipeline


@router.post("/{pipeline_id}/retry", response_model=PipelineResponse)
async def retry_pipeline(
    pipeline_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Retry a failed pipeline."""
    pipeline = next((p for p in MOCK_PIPELINES if p["id"] == pipeline_id), None)
    if not pipeline:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Pipeline not found",
        )

    if pipeline["status"] not in [PipelineStatus.FAILED, PipelineStatus.CANCELLED]:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Pipeline cannot be retried",
        )

    # Create a new pipeline run
    new_pipeline = {
        **pipeline,
        "id": str(len(MOCK_PIPELINES) + 1),
        "status": PipelineStatus.PENDING,
        "started_at": datetime.now(),
        "finished_at": None,
        "duration_seconds": 0,
        "current_stage": 0,
        "error_message": None,
    }
    MOCK_PIPELINES.append(new_pipeline)

    return new_pipeline
