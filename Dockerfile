FROM gcc:7.5
WORKDIR /usr/src/twclone
COPY source/* .
RUN autoconf
RUN aclocal
RUN automake --add-missing
RUN ./configure && make
RUN ./bigbang
CMD ./server 1234
EXPOSE 1234
