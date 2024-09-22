#!/bin/bash  
  
set -e  
  
# 构建目录  
BUILD_DIR="$(pwd)/build"  
if [ ! -d "$BUILD_DIR" ]; then  
    mkdir "$BUILD_DIR"  
fi  
  
# 清理并重新构建  
rm -rf "$BUILD_DIR"/*  

cd "$BUILD_DIR" &&
    cmake .. &&
    make

cd ..  
  
HEADER_DIR="/usr/include/mymuduo"  
if [ ! -d "$HEADER_DIR" ]; then  
    sudo mkdir -p "$HEADER_DIR"  
fi  
for header in *.h; do  
    sudo cp "$header" "$HEADER_DIR"  
done  
  
LIB_FILE="lib/libmyMuduo.so"  
if [ -f "$LIB_FILE" ]; then  
    sudo cp "$LIB_FILE" /usr/lib  
    sudo ldconfig  # 重新加载库文件配置  
else  
    echo "Error: Library file $LIB_FILE not found."  
    exit 1  
fi