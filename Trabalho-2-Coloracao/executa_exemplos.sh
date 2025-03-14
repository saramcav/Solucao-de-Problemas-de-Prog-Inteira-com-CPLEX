#!/bin/bash

make

echo "Arquivos passados: $@"

for arquivo in "$@"; do
    ./coloracao "$arquivo"
done