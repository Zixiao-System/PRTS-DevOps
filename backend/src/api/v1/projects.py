"""
Projects API endpoints.
"""

from datetime import datetime
from typing import Annotated, List, Optional

from fastapi import APIRouter, Depends, HTTPException, Query, status
from pydantic import BaseModel

from src.api.v1.auth import get_current_user

router = APIRouter()


# Request/Response Models
class ProjectCreate(BaseModel):
    """Project creation request."""

    name: str
    description: str
    team_id: str
    repository_url: Optional[str] = None


class ProjectUpdate(BaseModel):
    """Project update request."""

    name: Optional[str] = None
    description: Optional[str] = None
    team_id: Optional[str] = None
    repository_url: Optional[str] = None


class ProjectResponse(BaseModel):
    """Project response model."""

    id: str
    name: str
    description: str
    team_id: str
    team_name: str
    repository_url: Optional[str]
    language: str
    pipeline_status: str
    member_count: int
    branch_count: int
    open_issues: int
    created_at: datetime
    updated_at: datetime


class TeamResponse(BaseModel):
    """Team response model."""

    id: str
    name: str
    description: str
    member_count: int
    project_count: int


# Mock data
MOCK_PROJECTS: List[dict] = [
    {
        "id": "1",
        "name": "PRTS Web",
        "description": "运维管理平台前端项目",
        "team_id": "1",
        "team_name": "前端团队",
        "repository_url": "https://github.com/prts/web",
        "language": "TypeScript",
        "pipeline_status": "success",
        "member_count": 5,
        "branch_count": 12,
        "open_issues": 3,
        "created_at": datetime(2024, 1, 1),
        "updated_at": datetime(2024, 12, 20),
    },
    {
        "id": "2",
        "name": "PRTS API",
        "description": "运维管理平台后端 API 服务",
        "team_id": "2",
        "team_name": "后端团队",
        "repository_url": "https://github.com/prts/api",
        "language": "Python",
        "pipeline_status": "running",
        "member_count": 4,
        "branch_count": 8,
        "open_issues": 7,
        "created_at": datetime(2024, 1, 15),
        "updated_at": datetime(2024, 12, 22),
    },
    {
        "id": "3",
        "name": "PRTS Core",
        "description": "核心业务逻辑模块",
        "team_id": "3",
        "team_name": "核心团队",
        "repository_url": "https://github.com/prts/core",
        "language": "Go",
        "pipeline_status": "failed",
        "member_count": 3,
        "branch_count": 5,
        "open_issues": 12,
        "created_at": datetime(2024, 2, 1),
        "updated_at": datetime(2024, 12, 18),
    },
]

MOCK_TEAMS: List[dict] = [
    {"id": "1", "name": "前端团队", "description": "负责前端开发", "member_count": 5, "project_count": 3},
    {"id": "2", "name": "后端团队", "description": "负责后端开发", "member_count": 4, "project_count": 2},
    {"id": "3", "name": "核心团队", "description": "负责核心模块", "member_count": 3, "project_count": 1},
    {"id": "4", "name": "移动团队", "description": "负责移动端开发", "member_count": 2, "project_count": 1},
    {"id": "5", "name": "运维团队", "description": "负责运维工作", "member_count": 2, "project_count": 2},
]


@router.get("", response_model=List[ProjectResponse])
async def list_projects(
    current_user: Annotated[dict, Depends(get_current_user)],
    team_id: Optional[str] = Query(None, description="Filter by team ID"),
    search: Optional[str] = Query(None, description="Search by name or description"),
    skip: int = Query(0, ge=0),
    limit: int = Query(20, ge=1, le=100),
) -> List[dict]:
    """List all projects with optional filtering."""
    projects = MOCK_PROJECTS

    if team_id:
        projects = [p for p in projects if p["team_id"] == team_id]

    if search:
        search_lower = search.lower()
        projects = [
            p
            for p in projects
            if search_lower in p["name"].lower()
            or search_lower in p["description"].lower()
        ]

    return projects[skip : skip + limit]


@router.post("", response_model=ProjectResponse, status_code=status.HTTP_201_CREATED)
async def create_project(
    project_data: ProjectCreate,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Create a new project."""
    team = next((t for t in MOCK_TEAMS if t["id"] == project_data.team_id), None)
    if not team:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Team not found",
        )

    new_project = {
        "id": str(len(MOCK_PROJECTS) + 1),
        "name": project_data.name,
        "description": project_data.description,
        "team_id": project_data.team_id,
        "team_name": team["name"],
        "repository_url": project_data.repository_url,
        "language": "Unknown",
        "pipeline_status": "pending",
        "member_count": 0,
        "branch_count": 1,
        "open_issues": 0,
        "created_at": datetime.now(),
        "updated_at": datetime.now(),
    }
    MOCK_PROJECTS.append(new_project)

    return new_project


@router.get("/{project_id}", response_model=ProjectResponse)
async def get_project(
    project_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Get a specific project by ID."""
    project = next((p for p in MOCK_PROJECTS if p["id"] == project_id), None)
    if not project:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Project not found",
        )
    return project


@router.put("/{project_id}", response_model=ProjectResponse)
async def update_project(
    project_id: str,
    project_data: ProjectUpdate,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> dict:
    """Update a project."""
    project = next((p for p in MOCK_PROJECTS if p["id"] == project_id), None)
    if not project:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Project not found",
        )

    if project_data.name is not None:
        project["name"] = project_data.name
    if project_data.description is not None:
        project["description"] = project_data.description
    if project_data.team_id is not None:
        team = next((t for t in MOCK_TEAMS if t["id"] == project_data.team_id), None)
        if not team:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Team not found",
            )
        project["team_id"] = project_data.team_id
        project["team_name"] = team["name"]
    if project_data.repository_url is not None:
        project["repository_url"] = project_data.repository_url

    project["updated_at"] = datetime.now()

    return project


@router.delete("/{project_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_project(
    project_id: str,
    current_user: Annotated[dict, Depends(get_current_user)],
) -> None:
    """Delete a project."""
    global MOCK_PROJECTS
    project = next((p for p in MOCK_PROJECTS if p["id"] == project_id), None)
    if not project:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Project not found",
        )
    MOCK_PROJECTS = [p for p in MOCK_PROJECTS if p["id"] != project_id]


# Team endpoints
@router.get("/teams/", response_model=List[TeamResponse])
async def list_teams(
    current_user: Annotated[dict, Depends(get_current_user)],
) -> List[dict]:
    """List all teams."""
    return MOCK_TEAMS
