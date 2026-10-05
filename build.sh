#!/bin/sh
# Build the firmware in Docker and upload it with avrdude on the host.
#
#   ./build.sh                       build the default environment (Nano V3.0)
#   ./build.sh <env>...              build the given environments
#   ./build.sh all                   build all environments
#   ./build.sh upload <env> [port]   build, then flash over the serial bootloader
#   ./build.sh clean                 remove build output
#
# Environments: nanoatmega328, nanoatmega328new, miniatmega168
# Docker Desktop on macOS cannot pass USB serial ports to containers, so the
# upload runs avrdude on the host (macOS: brew install avrdude).
set -eu

cd "$(dirname "$0")"

IMAGE=arduino-matrix-orbital-pio
ALL_ENVS="nanoatmega328 nanoatmega328new miniatmega168"

die() { echo "error: $*" >&2; exit 1; }

build_image() {
  docker info >/dev/null 2>&1 || die "Docker is not running"
  docker build --quiet -t "$IMAGE" -f docker/Dockerfile . >/dev/null
}

pio() {
  docker run --rm --user "$(id -u):$(id -g)" -v "$PWD":/project "$IMAGE" "$@"
}

build() {
  build_image
  if [ $# -eq 0 ]; then
    pio run
  else
    for env in "$@"; do
      pio run -e "$env"
    done
  fi
}

# Bootloader settings from the PlatformIO board definitions
board_settings() {
  case "$1" in
    nanoatmega328)    MCU=atmega328p; BAUD=57600 ;;
    nanoatmega328new) MCU=atmega328p; BAUD=115200 ;;
    miniatmega168)    MCU=atmega168;  BAUD=19200 ;;
    *) die "unknown environment '$1' (expected one of: $ALL_ENVS)" ;;
  esac
}

find_port() {
  set -- /dev/cu.usbserial* /dev/cu.wchusbserial* /dev/cu.usbmodem* /dev/ttyUSB* /dev/ttyACM*
  found=""
  for p in "$@"; do
    [ -e "$p" ] && found="$found $p"
  done
  set -- $found
  [ $# -eq 1 ] || die "expected one serial port, found ${#}:${found:- none}; pass the port explicitly"
  echo "$1"
}

upload() {
  [ $# -ge 1 ] || die "usage: ./build.sh upload <env> [port]"
  env=$1
  board_settings "$env"
  command -v avrdude >/dev/null || die "avrdude not found (macOS: brew install avrdude)"
  port=${2:-$(find_port)}

  build "$env"
  avrdude -p "$MCU" -c arduino -P "$port" -b "$BAUD" -D \
    -U "flash:w:.pio/build/$env/firmware.hex:i"
}

case "${1:-}" in
  upload) shift; upload "$@" ;;
  clean)  rm -rf .pio ;;
  all)    build $ALL_ENVS ;;
  -h|--help) sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//' ;;
  *)      build "$@" ;;
esac
