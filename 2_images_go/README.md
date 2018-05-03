# Docker images reloaded

Let's go through another example of a Dockerfile.
This one is a little more complex as it involved multiple stages.

Many programming languages compile their source code.
In these cases, it is not required to put the sources into the production environment.
It is also unnecessary to have all the build tools available.
You can handle this with multi-stage builds.

## Check out the code

This time we build a simple Golang application.
It contains an upgraded website from the previous section.
The Go code to serve the site looks as follows:

```
package main

import (
	"log"
	"net/http"
)

func main() {
	fs := http.FileServer(http.Dir("static"))
	http.Handle("/", fs)

	log.Println("Listening...")
	http.ListenAndServe(":8000", nil)
}
```

You would compile this code with `go build -o out` to build a binary called `out`, then run it with `./out`.
Obviously, this requires Go installed.
It also seeks for the website assets in the folder `static`, relative to the location of the binary.
Make sure the assets in `app/static/` are in the correct place.

We will now write a Dockerfile that builds such a binary in a full-fledged build environment, then throws away everything to only keep the binary and nothing else.
To really build minimal size containers, we use some Golang tricks.
With `CGO_ENABLED=0` we make sure Go uses it's native implementation only and does not interact with C libraries in any way.
Using `go build -o app-bin -a -ldflags '-extldflags "-static"'` we get a binary that is statically compiled, i.e., one that does not require any libraries.
Together, the two settings ensure we have no OS dependencies at all beyond the Kernel, so we can use an empty container:

```
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
```

Note how we use two "FROM" statements, naming the first one "build", and using that name in the copy statements in the lower section.
The second "FROM scratch" statement refers to an empty image.
We only copy the compiled binary itself + the website assets, no OS files though.

## Build the image + run the container

You can now build with `docker build -t website:v2.0 .`.
Again, the base image is downloaded.
No other dependencies are required as we use the web server native to Go.
Run with `docker run -it --rm -p 8000:8000 website:v2.0` and the site should be available in your web browser.





