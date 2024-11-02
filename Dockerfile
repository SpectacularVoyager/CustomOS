FROM randomdude/gcc-cross-x86_64-elf

RUN apt-get update 
RUN apt-get upgrade -y
RUN apt-get install -y grub-common
RUN apt-get install -y nasm
RUN apt-get install -y xorriso
RUN apt-get install -y grub-pc-bin
RUN apt-get install -y grub-common
RUN apt-get install -y curl

VOLUME /root/os
WORKDIR /root/os

COPY src src
COPY ISO ISO
COPY linker.ld linker.ld
COPY Makefile Makefile
COPY scripts scripts
