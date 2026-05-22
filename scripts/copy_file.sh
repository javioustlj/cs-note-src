#!/bin/bash


declare -A SERVER_INFO
SERVER_INFO["10.10.66.183"]="arch Aii_biren_1234 /AII/ladc/tlj2"
SERVER_INFO["10.90.24.64"]="tlj tlj001 ~/file"
SERVER_INFO["10.90.24.66"]="tlj tlj001 ~/file"

copy_file_to_server() {
    local REMOTE_IP=$1

    if [[ -z "${SERVER_INFO[$REMOTE_IP]}" ]]; then
        echo "未找到 $REMOTE_IP 的服务器信息，请检查配置。"
        return 1
    fi


    local REMOTE_USER=$(echo "${SERVER_INFO[$REMOTE_IP]}" | cut -d' ' -f1)
    local REMOTE_PASSWORD=$(echo "${SERVER_INFO[$REMOTE_IP]}" | cut -d' ' -f2)
    local REMOTE_DIR=$(echo "${SERVER_INFO[$REMOTE_IP]}" | cut -d' ' -f3)

    echo "正在尝试将 $LOCAL_FILE 复制到 $REMOTE_USER@$REMOTE_IP:$REMOTE_DIR ..."
    # 使用 sshpass 进行文件复制
    sshpass -p "$REMOTE_PASSWORD" scp -o StrictHostKeyChecking=no "$LOCAL_FILE" "$REMOTE_USER@$REMOTE_IP:$REMOTE_DIR"

    if [ $? -eq 0 ]; then
        echo "文件成功复制到 $REMOTE_USER@$REMOTE_IP:$REMOTE_DIR"
        return 0
    else
        echo "文件复制到 $REMOTE_USER@$REMOTE_IP:$REMOTE_DIR 失败，请检查网络和认证信息。"
        return 1
    fi
}



if [ $# -lt 2 ]; then
    echo "用法: $0 <本地文件路径> <目标服务器IP1> [目标服务器IP2 ...]"
    exit 1
fi


LOCAL_FILE="$1"
shift  # 移除第一个参数，使得后续循环可以直接处理 IP 列表


if [ ! -f "$LOCAL_FILE" ]; then
    echo "本地文件 $LOCAL_FILE 不存在，请检查路径。"
    exit 1
fi

for REMOTE_IP in "$@"; do
    copy_file_to_server "$REMOTE_IP"
done
