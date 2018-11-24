FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install -y locales lighttpd nano

# Set the locale
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN mkdir -p /var/www/servers/docs
WORKDIR /var/www/servers

COPY lighttpd.conf .
COPY docs.tar.gz ./docs
RUN cd docs; tar xf docs.tar.gz

CMD lighttpd -Df lighttpd.conf
