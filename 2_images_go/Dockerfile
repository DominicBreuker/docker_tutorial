# build stage
FROM golang:1.10-alpine3.7 AS build

COPY app /go/src/app
WORKDIR /go/src/app

ENV CGO_ENABLED=0
RUN go build -o app-bin -a -ldflags '-extldflags "-static"'

# production stage
FROM scratch

COPY --from=build /go/src/app/app-bin /app-bin
COPY --from=build /go/src/app/static /static

CMD ["/app-bin"]

