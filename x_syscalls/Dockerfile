FROM debian:jessie

RUN apt-get update && apt-get install -y \
    gcc \
    vim \
    emacs

COPY src/ /src/
WORKDIR /src
CMD ["bash"]
