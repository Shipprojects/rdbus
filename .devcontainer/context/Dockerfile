# syntax=docker/dockerfile:1
FROM ubuntu:jammy

# Set timezone so apt could select correct region for download
RUN ln -snf /usr/share/zoneinfo/Europe/Riga /etc/localtime && echo Europe/Riga > /etc/timezone

COPY packages.txt download_packages.sh .
RUN sh download_packages.sh

COPY user_setup.sh .
RUN sh user_setup.sh

COPY nmea_tools_setup.sh .
RUN sh nmea_tools_setup.sh

COPY modbus_tools_setup.sh .
RUN sh modbus_tools_setup.sh

USER developer
WORKDIR /code
