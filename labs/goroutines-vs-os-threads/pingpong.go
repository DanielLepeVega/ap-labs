package main

import(
	"fmt"
	"time"
)
var count int
func pong(in chan bool, out chan bool){
	for {
		<- in
		// fmt.Println("pong")
		count++
		out <- false
	}
}
func ping(in chan bool, out chan bool){
	for {
		out <- true
		<- in
		// fmt.Println("ping")
		count++
	}
}

func main(){
	ch1 := make(chan bool)
	ch2 := make(chan bool)
	fmt.Println("Ping Pong started")
	go ping(ch1, ch2)
	go pong(ch2, ch1)
	for {
		time.Sleep(time.Second)
		fmt.Println("Comunications per second:",count)
		count = 0
	}
}