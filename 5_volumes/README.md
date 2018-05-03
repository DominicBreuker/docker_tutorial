# Volumes

There are two different ways you can persist state (=files) beyong the container lifetime:
- mount a host folder as a volume (`-v /path/on/host:/path/inside/container`)
- mount a Docker volume as a volume (`--mount source=myvol,target=/path/inside/container`)

The recommended way is to use the latter way.
Volumes are managed as resources by the Docker daemon.
Many different plugins allow mounting many different storage systems.

## How to use

- `docker volume create myvol`: create a volume called "myvol"
- `docker volume ls`: list all the volumes. You should see the one we just created.
- `docker volume inspect myvol`: inspect details of the volume
- `docker container run -it --rm --mount source=myvol,target=/myvol python sh`: run a container mounting this volume. Inside it, run `echo hello > /myvol/tmp.txt` to write a file into the volume. Then exit the container, which deletes it immediately due to the "--rm" flag.
- `docker container run -it --rm --mount source=myvol,target=/myvol python sh`: start a new container, again with the volume mounted. Run `cat /myvol/tmp.txt` to verify the file is still there. Then exit the container.
- `docker volume rm myvol`: Clean up and delete the volume. This actually destroys all it's files.


