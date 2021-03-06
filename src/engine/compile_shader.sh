#!/bin/bash

mkdir -p spv

glslc --version
if [[ $? -ne 0 ]]; then
  echo -e "\e[31mMissing GLSLC !!!\e[39m"
  exit 1
fi

pushd shaders >/dev/null 2>/dev/null
if [[ $? -ne 0 ]]; then
  echo -e "\e[31mMissing shaders !!!\e[39m"
  exit 1
fi

for SHADER in *; do
  echo -e "Compiling : \e[36m$SHADER\e[39m"
  glslc $SHADER -o ../spv/$SHADER.spv
  if [[ $? -ne 0 ]]; then
    echo -e "\e[31mError in : $SHADER\e[39m"
    exit 1
  fi
done