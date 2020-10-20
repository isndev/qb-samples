# qb-samples/elastic
Sample demonstrating how to use qb to create a (simple) centralized log database with it's dedicated log pusher ala [ELK](https://www.elastic.co/what-is/elk-stack)
## Client-log-watcher
Client will
- watch a log file
- parse and convert a log line from user format in json
- send json object(s) to the server

Usage
```sh
# ./qb-sample-elastic-client-log-watcher [FORMAT] [FILE_PATH] [HOST?] [PORT?]
```
* **Format** log format, ex: "(date):(file):(line):(message)"
* **File Path** file to watch
* **Host** remote host (default:localhost)
* **Port** remote port (default:60123)

## Server

Server will
* receive logs from watchers
* store it in database
* receive http request(s) and send back requested data

Usage
```sh
# ./qb-sample-elastic-server [CONFIG_FILE]
```

* **Config File** <br>
  - **N** JSONAcceptor - listen [interface:port], accept log watchers and forward sessions using a naive round robin to JSONSessionHandler(s)
  - **N** JSONSessionHandler - receive logs asynchronously and forward them to Database
  - **N** HTTPAcceptor - listen [interface:port], accept and forward sessions using a naive round robin to HTTPSessionHandler(s)
  - **N** HTTPSessionHandler - receive http requests, ask to database and send back requested logs
  - Database - store logs in memory
ex: 2 cores configuration
```json
{
  "JSONAcceptor": [
    {
      "core": 0,
      "listen": "0.0.0.0",
      "port": 60123
    }
  ],
  "HTTPAcceptor": [
    {
      "core": 0,
      "listen": "0.0.0.0",
      "port": 8080
    }
  ],
  "JSONSessionHandler": [{"core": 0}],
  "HTTPSessionHandler": [{"core": 0}],
  "Database": [{"core": 1}]
}
```
## Ask for Data
ex: curl asking for full database
```sh
# curl http://localhost:8080/get
```