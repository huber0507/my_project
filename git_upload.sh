#!/bin/bash

# 脚本功能：
# 1. 新建项目：自动初始化Git、关联远程仓库、提交并推送
# 2. 已有项目：自动拉取更新、提交变更、推送（与之前的同步功能一致）

# 提示用户输入远程仓库URL（新建项目必填，已有项目可选）
read -p "请输入远程仓库URL（如https://github.com/用户名/仓库名.git，已有项目可直接回车）: " REMOTE_URL

# 检查当前目录是否已初始化Git仓库
if [ ! -d ".git" ]; then
    echo -e "\n===== 当前目录未初始化Git仓库，开始初始化 ====="
    git init
    # 新建仓库默认分支为main（与GitHub默认一致）
    git checkout -b main
else
    echo -e "\n===== 检测到已初始化Git仓库 ====="
fi

# 处理远程仓库关联
if [ -n "$REMOTE_URL" ]; then
    # 如果用户输入了远程URL，强制关联（覆盖已有关联）
    echo -e "\n===== 关联远程仓库: $REMOTE_URL ====="
    git remote rm origin 2>/dev/null  # 忽略“远程不存在”的错误
    git remote add origin "$REMOTE_URL"
else
    # 未输入URL，检查是否已有远程关联
    if ! git remote | grep -q "origin"; then
        echo -e "\n错误：未检测到远程仓库关联，请重新运行脚本并输入远程URL！"
        exit 1
    fi
    REMOTE_URL=$(git remote get-url origin)
    echo -e "\n===== 已关联远程仓库: $REMOTE_URL ====="
fi

# 拉取远程最新代码（已有项目同步用，新建仓库首次拉取会提示“没有共同历史”，可忽略）
echo -e "\n===== 拉取远程最新更新 ====="
git pull origin main --allow-unrelated-histories
if [ $? -ne 0 ]; then
    echo "警告：拉取远程更新失败（可能是新建仓库无远程分支），将继续执行提交步骤..."
fi

# 检查本地变更
echo -e "\n===== 检查本地变更 ====="
git status
if git diff --quiet && git diff --cached --quiet; then
    echo "没有需要提交的变更，退出脚本。"
    exit 0
fi

# 添加所有文件到暂存区
echo -e "\n===== 添加所有变更到暂存区 ====="
git add .
if [ $? -ne 0 ]; then
    echo "错误：添加文件失败！"
    exit 1
fi

# 自定义提交信息
DEFAULT_COMMIT_MSG="Auto-sync at $(date +'%Y-%m-%d %H:%M:%S')"
read -p "请输入提交信息（默认: $DEFAULT_COMMIT_MSG）: " COMMIT_MSG
COMMIT_MSG=${COMMIT_MSG:-$DEFAULT_COMMIT_MSG}

# 提交变更
echo -e "\n===== 提交变更 ====="
git commit -m "$COMMIT_MSG"
if [ $? -ne 0 ]; then
    echo "错误：提交失败！"
    exit 1
fi

# 推送至远程仓库（首次推送需设置上游分支，后续推送无需）
echo -e "\n===== 推送到远程仓库 ====="
# 检查是否已设置上游分支
if git branch --show-current | grep -q "main" && ! git rev-parse --abbrev-ref --symbolic-full-name "@{u}" >/dev/null 2>&1; then
    # 首次推送：设置上游分支
    git push -u origin main
else
    # 非首次推送：直接推送
    git push origin main
fi

if [ $? -eq 0 ]; then
    echo -e "\n===== 操作完成！代码已成功同步到远程仓库 ====="
else
    echo -e "\n错误：推送失败，请检查网络或远程仓库权限！"
    exit 1
fi

