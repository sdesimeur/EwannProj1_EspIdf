#!/bin/bash

cat ./main/index.html | sed 's/\\/\\\\/g' | sed 's/"/\\"/g' | sed -z 's/\n/\\n" \\\n\t"/g' | sed '1s/^/#define INDEX_HTML "/' | sed '$s/$/"/' > ./main/index.h
