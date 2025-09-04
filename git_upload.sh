#!/bin/bash

# 检查当前目录是否为git仓库
if [ ! -d .git ]; then
    echo "错误：当前目录不是git仓库！"
    exit 1
fi

# 获取用户输入的提交信息（默认为带时间戳的消息）
read -p "请输入提交信息（默认：Auto-sync at $(date +'%Y-%m-%d %H:%M:%S')）：" commit_msg
commit_msg=${commit_msg:-"Auto-sync at $(date +'%Y-%m-%d %H:%M:%S')"}

# 执行git命令
echo "正在添加文件..."
git add .

echo "正在提交变更..."
git commit -m "$commit_msg"

# 检查提交是否成功（如果没有变更，commit会失败）
if [ $? -eq 0 ]; then
    echo "正在推送到GitHub..."
    git push origin main
    echo "推送完成！"
else
    echo "没有需要提交的变更，无需推送。"
fi

