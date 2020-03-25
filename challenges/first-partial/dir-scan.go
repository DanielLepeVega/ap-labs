package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func walkFn(path string, info os.FileInfo, err error) error {
	if err != nil {
		fmt.Printf("Error accessing path %q: %v\n", path, err)
		return err
	}

	switch mode := info.Mode(); true {
	case mode.IsDir():
		numDir++
	case mode&os.ModeSymlink != 0:
		numSL++
	case mode&os.ModeDevice != 0:
		numDev++
	case mode&os.ModeSocket != 0:
		numSock++
	default:
		numOthers++
	}
	return nil
}

// scanDirectory stands for the directory scanning implementation
func scanDirectory(dir string) error {
	err := filepath.Walk(dir, walkFn)
	if err != nil {
		return err
	}
	return nil
}

var numDir, numSL, numDev, numSock, numOthers int

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	err := scanDirectory(os.Args[1])
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	str := "+-----------------+----------+"
	fmt.Println("Directory Scanning Tool")
	fmt.Println(str)
	fmt.Printf("| Path            | %-9s|\n", os.Args[1])
	fmt.Println(str)
	fmt.Printf("| Directories     | %-9d|\n", numDir)
	fmt.Printf("| Symbolic Links  | %-9d|\n", numSL)
	fmt.Printf("| Devices         | %-9d|\n", numDev)
	fmt.Printf("| Sockets         | %-9d|\n", numSock)
	fmt.Printf("| Other files     | %-9d|\n", numOthers)
	fmt.Println(str)
}
