FROM debian:stable-20220912-slim as build

RUN apt update \
    && apt-get install --no-install-recommends -y \
        p7zip-full wget ca-certificates \
    && update-ca-certificates \
    && rm -rf /var/lib/apt/lists/*

ENV CHIBIOS_ARCHIVE_FNAME=ChibiOS_20.3.4.7z

# Get link from https://osdn.net/dl/chibios/ChibiOS_20.3.4.7z
RUN wget -O $CHIBIOS_ARCHIVE_FNAME \
    https://osdn.net/frs/redir.php?m=rwthaachen\&f=chibios%2F72607%2F$CHIBIOS_ARCHIVE_FNAME \
    && 7z x $CHIBIOS_ARCHIVE_FNAME \
    && rm $CHIBIOS_ARCHIVE_FNAME

RUN mv ChibiOS_20.3.4 chibios

FROM debian:stable-20220912-slim

RUN apt update \
    && apt-get install --no-install-recommends -y \
        make \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi \
    && rm -rf /var/lib/apt/lists/*

ENV CHIBIOS_ROOT=/chibios

COPY --from=build /chibios /chibios
