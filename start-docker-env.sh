#!/bin/sh

IMAGE="$1"
if [ -z "$IMAGE" ]; then
   IMAGE="ubuild"
fi

docker run --rm -v "$(pwd):/workspace" -ti "registry.iwstudio.hu/iws/$IMAGE:latest" bash -li

