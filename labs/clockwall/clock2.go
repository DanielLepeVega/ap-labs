// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"time"
)

func handleConnections(c net.Conn, local string) {
	defer c.Close()
	for {
		str := local + ":  " + time.Now().Format("15:04:05\n")
		_, err := io.WriteString(c, str)
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	local, err := time.LoadLocation("Local")
	if err != nil {
		log.Fatal("Error getting TZ")
	}
	if len(os.Args) < 3 {
		log.Fatal("Usage: go run clock2.go -port PORT")
	}
	listener, err := net.Listen("tcp", "localhost:"+os.Args[2])
	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}

		go handleConnections(conn, local.String()) // handle connections concurrently
	}
}
