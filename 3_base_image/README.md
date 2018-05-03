# Build an alpine base image

It is as simple as copying the file system to an empty container.
Check out the Dockerfile:
```
FROM scratch
ADD files/rootfs.tar.xz /
CMD ["/bin/sh"]
```

Here is the actual Alpine base image example: https://github.com/gliderlabs/docker-alpine/blob/61c3181ad3127c5bedd098271ac05f49119c9915/versions/library-3.7/x86_64/rootfs.tar.xz

Alpine Linux also offers a Mini Root Filesystem for download on its homepage: https://alpinelinux.org/downloads/

## Check out the filesystem

Go into the files folder and run `mkdir -p rootfs && tar -xf rootfs.tar.xz -C ./rootfs.tar.xz` and you will find the files extracted to the folder `rootfs`.
Browse the files and you will find a basic Linux filesystem.

## Build and use the image

Run `docker build -t local/alpine .` and you get the image.
Then you can do `docker run -it --rm local/alpine sh` and you get a shell inside an Alpine Linux container.
