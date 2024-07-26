FROM docker.io/debian:bookworm as builder

#RUN dnf update -y
RUN apt-get update -y \
    && apt-get install -y g++ coinor-libcbc-dev coinor-libcbc3 \
    && mkdir /workspace

WORKDIR /workspace

COPY stock_cutter/ /workspace
RUN g++ *.cpp -I. -I /usr/include/coin/  -L/usr/lib64/ -lCbc -lCbcSolver -lCgl -lClp -lCoinUtils -lOsi -lOsiClp -o stock_cutter


FROM docker.io/debian:bookworm

RUN apt-get update \
    && apt-get install -y coinor-libcbc3 \
    && apt-get clean all \
    && mkdir -p /app

WORKDIR /app

COPY --from=builder /workspace/stock_cutter /app/stock_cutter

RUN chmod 755 /app/stock_cutter

ENTRYPOINT [ "/app/stock_cutter" ]
CMD [ "example" ]
