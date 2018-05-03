# Docker Swarm

With Docker swarm, you can run Docker Containers on a cluster of multiple nodes.
Each node will run a few of the containers.
Docker ensures that most of the cluster management is taken care of.
Clusters consist of manager nodes, which are used to manage the cluster, and worker nodes, which only run workloads but cannot manage anything.

## Building a cluster

You can run a cluster on your local machine.
However, Swarm is really meant to be run on multiple machines which together form the cluster.
I recommend to give https://labs.play-with-docker.com/ a try for this one as you can create as many machines as you want in the lab environment.

Run the following commands:
- `docker swarm init --advertise-addr 127.0.0.1:2377 --listen-addr 127.0.0.1:2377`: initialize this node as a cluster manager. You will become the leader of this cluster. Specify the local address and port you listen on. Also specify the address and port to advertise to other nodes (usually the same, but could be different if you are behind a proxy)
- `docker node ls`: show the nodes of your swarm
- `docker swarm join-token manager`: show the join token for a new manager
- `docker swarm join-token worker`: show the join token for a new worker
- Now create as many new instances as you want and use the commands from above to add more workers and managers to your swarm.
- `docker swarm leave`: run this on any node to leave a swarm. Maybe add "--force" if you really want to leave and ignore warnings from Docker.

## Running services in the swarm

A swarm is for running services on top of it.
You can do this locally with only a single node.
It can be done as follows:
- `docker swarm init --advertise-addr 127.0.0.1:2377 --listen-addr 127.0.0.1:2377`: start your swarm
- `docker service create --name website -p 8000:8000 --replicas 2 website:v1.0`: run two instances of our website from an earlier example
- `docker service ls`: show a list of all the services
- `docker service ps website`: show some details for the website service
- `docker service inspect --pretty website`: show even more details on the service configuration
- `docker service scale website=3`: scale up the service to 3 instances
- `docker service update website --image website:v2.0`: update the service to the 2nd version. Swarm will apply a rolling update, upadting service instances one after the other. During the deploy, get `localhost:8000` very often and you may see either the old or new version, depending on which service you hit.
- `docker service rm website`: delete the service

## Docker swarm secrets

Swarm has an encrypted key value store for the cluster state.
It also allows to manage secrets.
This can be done as follows:
- `docker swarm init --advertise-addr 127.0.0.1:2377 --listen-addr 127.0.0.1:2377`: start up our swarm
- `echo password | docker secret create pw -`: create a new secret password
- `docker secret ls`: list all the available secrets
- `docker secret inspect pw`: inspect the password secret
- `docker service create --name app --secret pw local/node`: create a service with access to the secret
- Inside container: run `cat /run/secrets/pw` to verify that you can read the secret
- `docker swarm leave --force` to destroy the swarm, and the service with it.





