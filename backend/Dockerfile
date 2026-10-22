FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates cmake g++ make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN cmake -S . -B /build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target HomeTrainer --parallel

FROM debian:bookworm-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /build/HomeTrainer /app/HomeTrainer
COPY resources/ /app/resources/

ENV HOME_TRAINER_HOST=0.0.0.0
EXPOSE 8080
USER 10001:10001

ENTRYPOINT ["/app/HomeTrainer"]
