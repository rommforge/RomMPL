# Dockerized udpfsd (https://github.com/pcm720/udpfsd), the disk-serving sidecar
# for RomMPL. Serves RomM's PS2 folder read-only over UDPFS/UDPBD so Neutrino can
# boot games over the LAN by filename.
#
# This builds the CSO/ZSO path (static, no CGO). For on-the-fly CHD support,
# switch to a CGO-enabled build with libchdr per the upstream README.
FROM golang:1.25-alpine AS build
RUN apk add --no-cache git
WORKDIR /src
RUN git clone --depth 1 https://github.com/pcm720/udpfsd .
RUN CGO_ENABLED=0 go build -o /out/udpfsd ./cmd/udpfsd

FROM alpine:3.20
COPY --from=build /out/udpfsd /usr/local/bin/udpfsd
# udpfsd reads FSROOT / RO / PORT from the environment (or flags). Defaults here
# serve /fsroot read-only on the standard UDPBD port.
ENV FSROOT=/fsroot RO=true PORT=62966
EXPOSE 62966/udp
ENTRYPOINT ["udpfsd"]
