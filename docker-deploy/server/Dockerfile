FROM ubuntu:20.04
RUN mkdir /code
ADD . /code/
WORKDIR /code

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN ls 
RUN apt-get update 
RUN apt-get -y install gcc g++
RUN apt-get -y install postgresql postgresql-contrib
RUN apt-get -y install build-essential libpqxx-dev libpq-dev bc libpugixml-dev

RUN make clean && make
RUN chmod u+r+x run.sh

