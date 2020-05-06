// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"time"
	"strings"
)

//!+broadcaster
type clientChannel chan<- string // an outgoing message channel
type client struct {
	channel clientChannel//clientchannel
	name string//name of client
	ip string//ip of client
	conn net.Conn//connection
	admin bool//is the client admin
}

var (
	entering = make(chan client)//incoming messages
	leaving  = make(chan clientChannel)//outgoing messages
	clients  = make(map[clientChannel]*client) // all clients connected to the server
	messages = make(chan string) // all incoming client messages
)

func broadcaster() {
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case clientInformation := <-entering://joinning the server
			if len(clients) == 0 {
				clientInformation.admin = true//first client as admin
			}
			clients[clientInformation.channel] = &clientInformation//info of every client

		case cli := <-leaving://leaving the server
			delete(clients, cli)//delete clients at the end
			close(cli)//close channels
		}
	}
}
func admin(who string, cli clientChannel) {//turns a user into an admin
	fmt.Println(serverInformation + "["+ who + "] has been promoted as the master ADMIN")
	clients[cli].admin = true
	cli <- serverInformation + "You're the new IRC Server MASTER-ADMIN"
}

func clientChat(conn net.Conn, ch <-chan string) {//function so that the client can write
	for msg := range ch {
		fmt.Fprintln(conn, msg) //Ignores network errors
	}
}


func handleConn(conn net.Conn) {
	var username = false//bool var to check if a username is repeated
	input := bufio.NewScanner(conn)//read conn
	input.Scan()//read input
	who := input.Text()
	for _,person := range clients {
		if person.name == who {
			username = true//username is repeated
			fmt.Fprintln(conn, "Sorry the username you choosed is already taken, please try with a different one") // this only checks the username, it ignores network errors
			conn.Close()
			break;
		}
	}

	if !username {
		ch := make(chan string) // outgoing client messages
		go clientChat(conn, ch)
		ip := conn.RemoteAddr().String()//add connection to ip
		
		ch <- serverInformation + "Welcome to the Simple IRC Lepe Server"
		ch <- serverInformation + "The user [" + who + "] has successfully logged in"
		messages <- serverInformation + "New user: [" + who + "] has connected to the Lepe server"
		fmt.Println(serverInformation + "New user: [" + who + "] has connected to the Lepe server")
		
		entering <- client{ch, who, ip, conn, false}
		// Check if admin when joined (first user)
		if clients[ch].admin == true{
			ch <- serverInformation + "Congratulations, you were the first user to sign in the Lepe server."
			admin(who, ch)
		}

		for input.Scan() { // Incoming client messages
			msg := input.Text()
			if msg != "" {
				if msg[0] == '/' { // Command
					values := strings.Split(msg, " ")
					switch values[0] {//check all the possible commands
					case "/users":
						var user_name string
						for _,p := range clients{
							user_name += p.name + ", "
						}
						ch <- serverInformation + user_name[:len(user_name)-2]
					case "/msg":
						if len(values) < 3 {
							ch <- serverInformation + "Error parameters not in order, usage: /msg <user> <msg>"
						} else {
							var found = false
							for _,p := range clients {
								if p.name == values[1] {
									found = true
									p.channel <- who + " (direct) > " + msg[6+len(p.name):]
									break;
								}
							}
							if !found {
								ch <- serverInformation + "Error, user not found"
							}
						}
					case "/time":
						// Ingoring potential errors from time.LoadLocation("Local")
						location,_ := time.LoadLocation("Local")
						loc := location.String()
						// If TZ not specified load with America/Mexico_City
						if loc == "Local"{
							curr,_ := time.LoadLocation("America/Mexico_City")
							loc = curr.String()
						}
						ch <- serverInformation + "Local Time: " + loc + " " + time.Now().Format("15:04")
					case "/user":
						if len(values) != 2 {
							ch <- serverInformation + "Error in parameters, usage: /user <user>"
						} else {
							var found = false
							for _,person := range clients {
								if person.name == values[1] {
									found = true
									ch <- serverInformation + "username: " + person.name + ", IP: " + person.ip
									fmt.Println(serverInformation + "User: [" + who + "]")
									break;
								}
							}
							if !found {
								ch <- serverInformation + "Error, user not found"
							}
						}
					case "/kick":
						if clients[ch].admin {
							if len(values) != 2 {
								ch <- serverInformation + "Error in parameters, usage: /kick <user>"
							} else {
								var found = false
								for _,person := range clients {
									if person.name == values[1] {
										found = true
										person.channel <- serverInformation + "You're kicked from this channel"
										leaving <- person.channel
										fmt.Println(serverInformation + person.name + " has been kicked")
										messages <- serverInformation + person.name + " has been kicked from channel"
										person.conn.Close()
										break;
									}
								}
								if !found {
									ch <- serverInformation + "Error, user not found"
								}
							}
						} else {
							ch <- serverInformation + "Error, you don't have permissions, you need to be admin"
						}
					default:
						ch <- serverInformation + "Error, command not found"
					}
				} else { // Message
					messages <- who + " > " + msg
				}
			}
		}
		if clients[ch] != nil {
			// NOTE: ignoring errors that can happen in input.Err()
			leaving <- ch
			fmt.Println(serverInformation + "[" + who + "] left the chat")
			messages <- serverInformation + "[" + who + "] left the chat/channel"
			conn.Close()
		}
	}
}

var serverInformation string

func main() {
	if len(os.Args) != 5 {
		log.Fatal("Error introducing the parameters the correct way is, usage: go run client.go -host [host] -port [port]")
	}
	serverInformation = "irc--lepe--server > "
	server := os.Args[2] + ":" + os.Args[4]
	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(serverInformation + "Simple IRC Server started at " + server)
	go broadcaster()
	fmt.Println(serverInformation + "Ready for receiving new clients")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}
