FROM node:9-alpine

RUN apk add --no-cache curl

COPY app /app
WORKDIR /app

RUN npm install

CMD ["node", "app.js"]


