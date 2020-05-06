// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"fmt"
)

//!+
func main() {
	if len(os.Args) != 5 {
		log.Fatal("Error with parameters, usage: go run client.go -user [user] -server [server]")
	}
	user := os.Args[2]
	server := os.Args[4]
	conn, err := net.Dial("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Fprintln(conn, user)
	done := make(chan bool)
	go func() {
		io.Copy(os.Stdout, conn) // Ignoring possible errors
		log.Println("procces is done")
		done <- true // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	_, err := io.Copy(dst, src)
	if err != nil {
		log.Fatal(err)
	}
}