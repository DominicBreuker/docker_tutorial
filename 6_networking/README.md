# Networking

Networking allows you to create your favorite network topology for your containers, all with simple Docker commands.
Let's go through a very simple example with 3 containers and 2 networks.
Container 1 is in the first network, container 3 in the second.
Container 2 shall be in both networks.
We will verify that container 2 can reach all other containers, but containers 1 and 3 can not talk to each other.

## Setup

- `docker network create -d bridge mynet`: create a new network called "mynet"
- `docker network create -d bridge myothernet`: create a second network called "myothernet"
- open 3 terminal windows and run:
- `docker container run -it --rm --name c1 --network mynet alpine sh`: create container 1
- `docker container run -it --rm --name c2 --network mynet alpine sh`: create container 2
- `docker container run -it --rm --name c3 --network mynet alpine sh`: create container 3
- Inside c2: `ping <ip_c1>` and `ping c1` should work, `ping c3` should not. Note we can use the IP or DNS name for the containers. You can find out the IP for instance with `docker container inspect c1`.
- `docker network connect myothernet c2`: now connect container "c2" also to "myothernet"
- Inside c2: `ping <ip_c1>` and `ping c1` should work, `ping c3` should now work as well
- Inside c3: `ping c1` does not work
- Inside c1: `ping c3` does not work

Don't forget to clean up when you are done.
