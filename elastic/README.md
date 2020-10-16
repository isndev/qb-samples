# qb-samples
## Elastic
Sample demonstrating how to use qb to create a (simple) centralized log database with it's dedicated log pusher aka [ELK](https://www.elastic.co/what-is/elk-stack)
* ### client-log-watcher

Client will
* watch a log file
* parse and convert a log line from user format in json
* send json object(s) to the server

Usage
```sh
# ./qb-sample-elastic-client-log-watcher [FORMAT] [FILE_PATH] [HOST?] [PORT?]
```
* **Format** log format, ex: "(date):(file):(line):(message)"
* **File Path** file to watch
* **Host** remote host (default:localhost)
* **Port** remote port (default:60123)

* ### server

Server will
* receive logs from watchers
* store it in database
* receive http request(s) and send back requested data

Usage
```sh
# ./qb-sample-elastic-server
```

###todo