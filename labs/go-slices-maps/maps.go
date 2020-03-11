package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
	arr := strings.Fields(s)
	mapa := make(map[string]int)
	for _, v := range arr {
		mapa[v] += 1
	}
	return mapa
}

func main() {
	wc.Test(WordCount)
}
