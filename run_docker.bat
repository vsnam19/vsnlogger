@echo off
echo VSNLogger Docker Development Environment Setup
echo ================================================

REM Check if Docker is running
docker info >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Docker is not running. Please start Docker Desktop and try again.
    exit /b 1
)

REM Set image name and file paths
set IMAGE_NAME=vsnlogger-dev
set SOURCE_DIR=C:\Users\vsnam\Workspaces\Work\vsnlogger
set DOCKER_DIR=/app

REM Check if the source directory exists
if not exist "%SOURCE_DIR%" (
    echo Error: Source directory "%SOURCE_DIR%" does not exist.
    exit /b 1
)

echo Building Docker image...
docker build -t %IMAGE_NAME% -f "%SOURCE_DIR%\Dockerfile" "%SOURCE_DIR%"
if %errorlevel% neq 0 (
    echo Error: Failed to build Docker image.
    exit /b 1
)

echo Starting Docker container with volume binding...
echo   Local: %SOURCE_DIR%
echo   Container: %DOCKER_DIR%

REM Run the container with interactive terminal and volume binding
docker run -it --rm ^
    --name vsnlogger-container ^
    -v "%SOURCE_DIR%:%DOCKER_DIR%" ^
    %IMAGE_NAME%

echo Container exited.
