# syntax=docker/dockerfile:1
FROM ubuntu:jammy
COPY packages.txt download_packages.sh .
RUN sh download_packages.sh

COPY user_setup.sh .
RUN sh user_setup.sh

USER developer
WORKDIR /code
