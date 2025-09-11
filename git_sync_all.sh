#!/bin/bash

# 配置区域 - 根据你的实际情况修改
PROJECT_PATH="$HOME/linux/my_project"  # 项目本地路径
REMOTE_BRANCH="main"                   # 远程分支名称（通常是main或master）
REMOTE_NAME="origin"                   # 远程仓库名称（默认origin）

# 进入项目目录
echo "===== 进入项目目录: $PROJECT_PATH ====="
cd "$PROJECT_PATH" || {
    echo "错误：无法进入项目目录，请检查PROJECT_PATH配置是否正确！"
    exit 1
}

# 第一步：从远程仓库拉取最新代码（优先同步远程更新）
echo -e "\n===== 开始从远程拉取最新更新（分支：$REMOTE_BRANCH） ====="
git pull "$REMOTE_NAME" "$REMOTE_BRANCH"
if [ $? -ne 0 ]; then
    echo "警告：拉取远程更新失败！可能是网络问题或冲突，请手动处理后重试。"
    # 拉取失败仍继续尝试推送本地变更（可选：若想严格同步，可将下方exit注释取消）
    # exit 1
fi

# 第二步：检查本地是否有需要提交的变更
echo -e "\n===== 检查本地代码变更 ====="
git status
# 检查工作区和暂存区是否有变更
if git diff --quiet && git diff --cached --quiet; then
    echo "本地没有需要提交的变更，同步完成。"
    exit 0
fi

# 新增：自动添加所有未跟踪文件（排除.gitignore中定义的文件）
echo "===== 自动添加未跟踪文件到暂存区 ======"
# git add . 会添加所有未跟踪文件（但会遵守.gitignore规则，忽略不需要的文件）
git add .
# 检查是否有未跟踪文件被添加成功
added_files=$(git status --porcelain | grep '^A')
if [ -n "$added_files" ]; then
    echo "已自动添加以下未跟踪文件："
    echo "$added_files"
else
    echo "无未跟踪文件需要添加"
fi

# 第三步：自动添加所有变更到暂存区
echo -e "\n===== 自动添加所有变更到暂存区 ====="
git add .
if [ $? -ne 0 ]; then
    echo "错误：添加文件到暂存区失败！"
    exit 1
fi

# 第四步：获取提交信息（默认带时间戳，支持自定义）
DEFAULT_COMMIT_MSG="Auto-synced at $(date +'%Y-%m-%d %H:%M:%S')"
read -p "请输入提交信息（默认：$DEFAULT_COMMIT_MSG）: " COMMIT_MSG
COMMIT_MSG=${COMMIT_MSG:-$DEFAULT_COMMIT_MSG}  # 若用户未输入则用默认

# 第五步：提交本地变更
echo -e "\n===== 提交本地变更 ====="
git commit -m "$COMMIT_MSG"
if [ $? -ne 0 ]; then
    echo "错误：提交变更失败！"
    exit 1
fi

# 第六步：推送到远程仓库
echo -e "\n===== 推送到远程仓库（分支：$REMOTE_BRANCH） ====="
git push "$REMOTE_NAME" "$REMOTE_BRANCH"
if [ $? -eq 0 ]; then
    echo -e "\n===== 双向同步完成！本地变更已上传，且已同步远程最新代码 ====="
else
    echo -e "\n错误：推送失败！请检查网络或GitHub权限（如PAT是否有效）。"
    exit 1
fi
