# Docker Engine

The Docker Engine is responsible for running containers on a system.
You can interact with it through your Docker client installed on the system.

## Basic workflow

Follow this sample workflow:
- `docker container ps -a`: show a list of all containers, currently running and stopped. Should be empty initially.
- `docker container run -it python:latest sh`: start a container from the image "python:latest" and execute "sh" inside to start a shell. Thanks to using the "-it" flags, you can interact with this shell. Explore and realize you seem to be on a normal Linux server which has Python installed.
- `docker container run -it -v $(pwd)/app:/app python:latest python /app/main.py`: start a Python container and mount the folder "app" from the current directory into it. Then run "python /app/main.py" to execute the Python program we have just mounted.
- `docker container run -it -e VAR=env-var-value python python -c 'import os; print(os.environ["VAR"])'`: start a Python container and instruct Docker to make an environment variable available "VAR" inside the container. Then execute an inline Python script to print out this variable to prove it is actually there. Note we did not specify ":latest" for the image name, but Docker adds it automatically.
- `docker container ps -a`: show all the containers. You should now see 3 containers, corresponding to the 3 commands previously executed.
- `docker container ps -aq`: the same command as before, but this time print only the IDs of these containers.
- `docker container rm $(docker container ps -aq)`: remove all containers - nice cleanup command worth remembering.

These are the basics of working with containers. 
Many more commands are available. 
Check out the docs for details: https://docs.docker.com/engine/reference/run/

## Running a server

Now try running a real web server inside the container:
- `docker container run -t -d -p 8000:1234 python:latest /bin/sh -c 'cd /tmp && echo "hello world" > /tmp/index.html  && exec python -m http.server 1234'`: start a Python container which runs in background ("-d") with tty attached ("-t") and exposing port 1234 inside the container on port 8000 of your computer. Upon startup, execute a shell script that creates a simple website and uses Python to serve it on port 1234. When done, go to `localhost:8000` in your browser and you should see your website. Stop the server with CTRL+c.
- `docker container ps`: run this to find out the ID of the running container.
- `docker container logs -f <container_id>`: use this command to read the web server logs of your container.
- `docker exec -it <container_id> sh`: use this command to get a shell inside the running container. Yes, you do not have to start a shell when starting the container. At any time, you can execute additional commands inside them.
- `docker container kill <container_id>`: use this command to explicitly kill the container, for instance if CTRL+c does not work. You could use `stop` rather than `kill` to gracefully stop the container.

## Container lifecycle

Let's now look into the lifecycle of a container.
They follow the design of normal services found, e.g., on Linux and Windows machines.
This means you can start, stop, kill, ... them.

Each container comes with state.
It starts with it's initial state taken from the image it is based on and works from there.
Any change you make, such as creating a file, is persistent for the life of this container.
However, as you start another container from the same image, it does start with the initial state again.
When you delete a container, the state is lost forever.

Run the following commands:
- `docker container run --name mypython -t -d -p 8000:1234 python /bin/sh -c 'cd /tmp && echo "hello world" > /tmp/index.html  && exec python -m http.server 1234'`: starts the web server we used before. Note the "--name" flag we use to give the container an human-readable name. It allows us to refer to it via name, not ID.
- Optional: in a different shell, run `docker container logs -f mypython` to follow your web server logs.
- Check out `curl localhost:8000` to see the normal "hello world" message. Try out `curl localhost:8000/data.txt` and you will get a 404, indicating the file does not exist on the server.
- Now run `docker container exec mypython /bin/sh -c ‘echo “data” > /tmp/data.txt’` to create the "data.txt" file.
- If you now try `curl localhost:8000/data.txt`, your file will be there.
- `docker container stop mypthon`: stop the container. Now the website is not available anymore.
- `docker container start mypython`: restarts the container. You should now still be able to to `curl localhost:8000/data.txt` successfully since the file still exists. It survived stopping and starting the container.
- `docker container kill mypython`: kill the container (faster than stopping)
- `docker container rm mypython`: remove the container along with all it's state.

If you now start again from the beginning, you get a fresh version of the container without the "data.txt" file inside.





