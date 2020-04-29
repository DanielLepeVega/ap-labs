package main

import (
	"io"
	"log"
	"net"
	"os"
)

func copy(dst io.Writer, src io.Reader, done chan int) {
	if _, err := io.Copy(dst, src); err != nil { //coppy method
		log.Fatal(err)
	} //error recognition
	done <- 1
}

func main() {
	if len(os.Args) < 4 { //check length of Args
		log.Fatal("Usage: go run clockWall.go NewYork=localhost:PORT1 Tokyo=localhost:PORT2 London=localhost:PORT3")
	} //use log function to notify user
	done := make(chan int)
	for i := 1; i <= 3; i++ {
		str := os.Args[i] //initialize connection to the servers
		conn, err := net.Dial("tcp", "localhost:"+str[len(str)-4:])
		if err != nil {
			log.Fatal(err) //error recognition
		}
		go copy(os.Stdout, conn, done) //go function to copy, check previous methods
	}
	<-done
}
