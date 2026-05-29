FROM ubuntu:latest
LABEL authors="martin"

ENTRYPOINT ["top", "-b"]