# Docker compose

Using the Docker API is cool, but not terribly reproducible.
For complex stacks, you have to run a lot of commands.
Better is to use Docker compose to specify what the stack looks and let the tool orchestrate all the calls.
Let's write a simple docker-compose.yml file to run a stack consisting of two services:
- Website: simple Python-based website with a visit counter
- Redis: cache to store state

## Check out the code

The Python website for this example is not as shiny as the ones before, but it now remembers stuff.
For that, it connects to a Redis and increases a counter `hits` each time somebody requests the website.
The code looks as follows:
```python
import time

import redis
from flask import Flask

app = Flask(__name__)

redis_host = os.getenv("REDIS_HOST")
cache = redis.Redis(host=redis_host, port=6379)

def get_hit_count():
    retries = 5
    while True:
        try:
            return cache.incr('hits')
        except redis.exceptions.ConnectionError as exc:
            if retries == 0:
                raise exc
            retries -= 1
            time.sleep(0.5)

@app.route('/')
def hello():
    count = get_hit_count()
    return 'Hi! I have been called {} times\n'.format(count)

if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
```

Note that we respond to each request by getting the hit count from Redis and increasing it in the process, as an atomic operation.
We let the application know where Redis is by fetching it's location from an environment variable.

We also need a Dockerfile for this application:
```docker
FROM python:3.6-alpine
COPY . /code
WORKDIR /code
RUN pip install -r requirements.txt
CMD ["python", "app.py"]
```

This time, we need two Python dependencies: flask as a web server and the client for Redis.
These are installed with pip, a Python package management tool.
Afterwards, we define how to start the app and are done.

Now we define the docker-compose.yml file.
```docker-compose
version: '3'

services:
  web:
    build: ./app
    environment:
        REDIS_HOST: "redis"
    ports:
     - "8000:5000"

  redis:
    image: "redis:alpine"
```

The file defines a version (enables certain features of Docker compose).
It also lists two services named "web" and "redis".
For the service "web", we define that it should be built from a Dockerfile located in "./app", relative to the compose file.
It shall have an environment variable "REDIS_HOST" to let the Python app know how to find Redis.
Due to Docker's DNS feature, the simple name "redis" works (in Docker compose, the service names as defined in the YML can be used).
We do not need an IP.
We furthermore specify we expose port 5000 inside the container, which is Flasks default port, as port 8000 locally.

The next service is redis, which is not built from a local Dockerfile.
Rather, we just fetch this service from Dockerhub, specifying "redis:alpine" as the image.

## Start the stack

To start the stack, we do not have to build our Docker image first.
Rather, the only thing we do is run `docker-compose up` and the tool does the rest for us.
You will see all the logs printed out on the terminal, nicely colored by service.
Loading `localhost:8000` multiple times will increase the counter one by one.
CTRL+c will stop the services.

If you start the stack with `docker-compose up -d`, it will start in the background.
Run afterwards `docker container ps` and you see two running containers.
Note this command is not a Docker compose command.
Docker compose merely orchestrates calls to the Docker daemon.
All normal commands can be used.

As for compose-specific commands:
- `docker-compose ps`: see a list of your containers
- `docker-compose top`: see which processes are running inside the containers.
- `docker-compose logs -f`: follow the logs of all your services

Finally, use `docker-compose down` to destroy the stack.







