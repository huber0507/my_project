#!/bin/bash

# 配置项目路径（请修改为你的my_project实际路径）
PROJECT_PATH="$HOME/linux/my_project"
# 远程仓库分支（默认main，根据实际情况修改，如master）
REMOTE_BRANCH="main"

# 进入项目目录
echo "===== 进入项目目录: $PROJECT_PATH ====="
cd "$PROJECT_PATH" || {
    echo "错误：无法进入项目目录，请检查路径是否正确！"
    exit 1
}

# 拉取远程最新代码（避免冲突）
echo -e "\n===== 拉取远程最新更新 ====="
git pull origin "$REMOTE_BRANCH"
if [ $? -ne 0 ]; then
    echo "警告：拉取远程更新失败，可能存在冲突，请手动处理后再运行脚本！"
    exit 1
fi

# 检查是否有需要提交的变更
echo -e "\n===== 检查本地变更 ====="
git status
if git diff --quiet && git diff --cached --quiet; then
    echo "没有需要提交的变更，退出脚本。"
    exit 0
fi

# 自动添加所有变更（包括新文件、修改、删除）
echo -e "\n===== 添加所有变更到暂存区 ====="
git add .
if [ $? -ne 0 ]; then
    echo "错误：添加文件失败！"
    exit 1
fi

# 获取默认提交信息（包含时间戳）
DEFAULT_COMMIT_MSG="Auto-sync at $(date +'%Y-%m-%d %H:%M:%S')"
# 提示用户输入自定义提交信息（按回车使用默认）
echo -e "\n===== 请输入提交信息 ====="
read -p "提交信息（默认: $DEFAULT_COMMIT_MSG）: " COMMIT_MSG
COMMIT_MSG=${COMMIT_MSG:-$DEFAULT_COMMIT_MSG}  # 若用户未输入，使用默认

# 提交变更
echo -e "\n===== 提交变更 ====="
git commit -m "$COMMIT_MSG"
if [ $? -ne 0 ]; then
    echo "错误：提交失败！"
    exit 1
fi

# 推送到远程仓库
echo -e "\n===== 推送到远程仓库 ====="
git push origin "$REMOTE_BRANCH"
if [ $? -eq 0 ]; then
    echo -e "\n===== 所有操作完成！代码已成功同步到远程仓库 ====="
else
    echo -e "\n错误：推送失败，请检查网络或远程仓库权限！"
    exit 1
fi

