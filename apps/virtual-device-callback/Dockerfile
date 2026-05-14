FROM ubuntu:20.04

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN apt-get update
RUN apt-get install -y git make gcc npm 
RUN apt-get install -y wget zip unzip fdisk nano curl xz-utils
RUN apt install snapd -y
RUN curl -s https://ngrok-agent.s3.amazonaws.com/ngrok.asc | tee /etc/apt/trusted.gpg.d/ngrok.asc >/dev/null && echo "deb https://ngrok-agent.s3.amazonaws.com buster main" | tee /etc/apt/sources.list.d/ngrok.list && apt update && apt install ngrok

WORKDIR /
RUN git clone https://github.com/SmartThingsCommunity/st-schema-callback-example-js.git
WORKDIR /st-schema-callback-example-js
RUN npm install
ADD .env .
ENTRYPOINT node server.js
ENTRYPOINT ngrok http 3000
