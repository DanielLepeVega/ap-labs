// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"fmt"
	"log"
	"os"
	"strings"
	"strconv"
	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)
var depth = -1
type Worklist struct {
	links []string
	current_depth int
}

func crawl(url string, current_depth int) Worklist {
	fmt.Println(url)
	// Freedom
	if current_depth == -1 { 
		tokens <- struct{}{} // acquire a token
		list, err := links.Extract(url)
		<-tokens // release the token

		if err != nil {
			log.Print(err)
		}
		return Worklist{list, current_depth}
	}
	// Depth Limit
	if current_depth < depth{
		tokens <- struct{}{} // acquire a token
		list, err := links.Extract(url)
		<-tokens // release the token

		if err != nil {
			log.Print(err)
		}
		return Worklist{list, current_depth + 1}
	}
	return Worklist{nil, depth}
}

//!-sema

//!+
func main() {
	if 	len(os.Args)<2 || 
	(strings.HasPrefix(os.Args[1], "-") && 
	(len(os.Args) < 3 || !strings.HasPrefix(os.Args[1], "-depth=") || len(os.Args[1]) < 8)) {
		fmt.Println("Parameters are incorrect, usage:\ngo run crawl3.go [-depth=<number>] <links>")
		return
	}
	if strings.HasPrefix(os.Args[1], "-depth="){
		// fmt.Println(os.Args[1][7:])
		i, err := strconv.Atoi(os.Args[1][7:])
		if err != nil {
			fmt.Println("It is not possible to convert to integer the value specified in -depth=(value)")
			return
		}
		if i <= 0 {
			fmt.Println("Depth has to be greater than 0")
			return
		}
		depth = i
		fmt.Println("Defined depth:", depth)
	}
	worklist := make(chan Worklist)
	var n int // number of pending sends to worklist

	// Start with the command-line arguments.
	n++
	if depth == -1 {  // Freedom
		go func() { worklist <- Worklist{os.Args[1:], -1} }()
	} else { // Depth limit
		go func() { worklist <- Worklist{os.Args[2:], 0} }()
	}

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	for ; n > 0; n-- {
		list := <-worklist
		if list.links != nil {
			for _, link := range list.links {
				if !seen[link] {
					seen[link] = true
					n++
					go func(link string) {
						worklist <- crawl(link, list.current_depth)
					}(link)
				}
			}
		}
	}
}
//!-