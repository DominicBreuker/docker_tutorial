# Docker images

Let us now look into Docker images.
As an example, we build a simple image that contains a static website.

## Check out the code

The website is built as a node.js application serving a set of static HTML, JS and CSS assets.
Check out `app/app.js` to see how it works, which contains this code:

```node
var http = require('http');

var finalhandler = require('finalhandler');
var serveStatic = require('serve-static');

var serve = serveStatic("./static/");

var server = http.createServer(function(req, res) {
  var done = finalhandler(req, res);
  serve(req, res, done);
});

process.on('SIGINT', function() {
    process.exit();
});

server.listen(8000);
```

We simply serve all files from the folder `static` and start a web server on port 8000.
To make our lives as developers easy, we use two libraries, which are specified as dependencies in `app/package.js`.
They must be installed with npm, the node package manager, before we can run the application.
On our computer, we would call `npm install` to install these dependencies, then `node app.js` to start the app.

All assets are in the folder `app/static/` and it is a pretty standard bootstrap template for a singe page website.

To dockerize the app, we must do these things inside the Dockerfile.
A simple Dockerfile looks like so:
```docker
FROM node:9-alpine

RUN apk add --no-cache curl

COPY app /app
WORKDIR /app

RUN npm install

CMD ["node", "app.js"]
```

We start from a Alpine Linux image with node.js version 9 installed.
This image is one of the images you can find here on DockerHub: ``https://hub.docker.com/_/node/``

Next we add curl to the programs installed in this image using `apk`, the Alpine package manager.
We do not need this program to run the web server but it illustrates nicely how system dependencies would be installed.
Note we use "--no-cache" option.
Many base images for Docker have all cache files removed for their package managers to reduce image size, so special tricks like this one are required to install stuff.

Finally, we now copy the source code and set the working directory.
Then we can run npm install to add the required libraries.
Lastly, we specify "node app.js" as the command to be run when starting containers from this image.

## Build the image + run the container

We can run `docker build -t local/website:v1.0 .` to build a new image based on the Dockerfile (the current directory must be the one where the Dockerfile is).
This will pull the base image from Dockerhub, then execute the steps defined in the Dockerfile to make the necessary changes.
Once that is done, we have a new image with tag `local/website:v1.0` available.
Run `docker image ls` to verify it is there.
Note how we use a version other than latest to build the image.

The next thing to do is run `docker run -it --rm -p 8000:8000 local/website:v1.0` to start our new container.
Visit `localhost:8000` afterwards to verify the website is actually served.






