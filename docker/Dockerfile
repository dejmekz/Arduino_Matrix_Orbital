# Build environment for the firmware: PlatformIO with the AVR toolchain.
# Used by build.sh; works on Apple Silicon without Rosetta.
FROM python:3.12-slim

ARG PIO_VERSION=6.2.0
RUN pip install --no-cache-dir --root-user-action=ignore platformio==${PIO_VERSION}

# Shared core dir, writable for any uid so build.sh can run as the host user
ENV PLATFORMIO_CORE_DIR=/opt/platformio
RUN mkdir -p ${PLATFORMIO_CORE_DIR} && chmod 777 ${PLATFORMIO_CORE_DIR}

# Pre-install the platform, framework and toolchain of every environment,
# so builds work offline and the image is rebuilt when platformio.ini changes
COPY platformio.ini /tmp/seed/platformio.ini
RUN cd /tmp/seed && mkdir matrix && pio pkg install && rm -rf /tmp/seed \
 && chmod -R a+rwX ${PLATFORMIO_CORE_DIR}

WORKDIR /project
ENTRYPOINT ["pio"]
