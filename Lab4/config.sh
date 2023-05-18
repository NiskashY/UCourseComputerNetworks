#!/bin/bash

current_dir=$(pwd)

declare -a dir_names=("client" "server" "server-communication")


for i in "${dir_names[@]}"
do
    cd $current_dir
    mkdir ./"$i"/build
    cd ./"$i"/build && cmake .. && cmake --build .
done

