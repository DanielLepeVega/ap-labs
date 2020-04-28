package main

import (
	"io"
	"log"
	"net"
	"os"
)

func copy(dst io.Writer, src io.Reader, done chan int) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
	done <- 1
}

func main() {
	if len(os.Args) < 4 {
		log.Fatal("Usage: go run clockWall.go NewYork=localhost:PORT1 Tokyo=localhost:PORT2 London=localhost:PORT3")
	}
	done := make(chan int)
	for i := 1; i <= 3; i++ {
		str := os.Args[i]
		conn, err := net.Dial("tcp", "localhost:"+str[len(str)-4:])
		if err != nil {
			log.Fatal(err)
		}
		go copy(os.Stdout, conn, done)
	}
	<-done
}
