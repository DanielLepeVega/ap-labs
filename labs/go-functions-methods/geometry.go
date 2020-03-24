// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/
// See page 156.
// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}
func (p Point) Y() float64 {
	return p.y
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

//!-point
//!+path
// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			curr := path[i-1].Distance(path[i])
			sum += curr
			fmt.Printf("%f + ", curr)
		}
	}
	curr := path[len(path)-1].Distance(path[0])
	sum += curr
	fmt.Printf("%f = %f\n", curr, sum)
	return sum
}

//!-path
func onSegment(p, q, r Point) bool {
	if q.X() <= math.Max(p.X(), r.X()) && q.X() >= math.Min(p.X(), r.X()) &&
		q.Y() <= math.Max(p.Y(), r.Y()) && q.Y() >= math.Min(p.Y(), r.Y()) {
		return true
	}
	return false
}

func orientationSide(p, q, r Point) int {
	var value int = int((q.Y()-p.Y())*(r.X()-q.X()) -
		(q.X()-p.X())*(r.Y()-q.Y()))
	if value == 0 {
		return 0
	}
	if value > 0 {
		return 1
	}
	return 2
}

func validate(x int) bool {
	if x == len(p) {
		for i := x - 2; i > 1; i-- {
			if !valueID(p[x-1], p[0], p[i], p[i-1]) {
				//fmt.Printf("Failed at last %d with %d\n",x,i)
				return false
			}
		}
		return true
	}
	for i := x - 2; i > 0; i-- {
		if !valueID(p[x], p[x-1], p[i], p[i-1]) {
			//fmt.Printf("Failed at %d with %d\n",x,i)
			return false
		}
	}
	return true
}

func valueID(p1, q1, p2, q2 Point) bool {
	o1 := orientationSide(p1, q1, p2)
	o2 := orientationSide(p1, q1, q2)
	o3 := orientationSide(p2, q2, p1)
	o4 := orientationSide(p2, q2, q1)
	if o1 != o2 && o3 != o4 {
		//fmt.Printf("orientationSide ")
		return false
	}
	if o1 == 0 && onSegment(p1, p2, q1) {
		return false
	}
	if o2 == 0 && onSegment(p1, q2, q1) {
		return false
	}
	if o3 == 0 && onSegment(p2, p1, q2) {
		return false
	}
	if o4 == 0 && onSegment(p2, q1, q2) {
		return false
	}
	return true
}

func createSides(n int) {
	p = make([]Point, n)
	rand.Seed(time.Now().UTC().UnixNano())
	var valueid bool = false
	for !valueid {
		for i := range p {
			// create unique point
			for !point(i) {
			}
			//fmt.Printf("(%.f , %.f)\n",p[i].X(),p[i].Y())
			valueid = validate(i)
			if !valueid {
				break
			}
		}
		if valueid {
			valueid = validate(len(p))
		}
	}
	for i := range p {
		fmt.Printf("(%.f , %.f)\n", p[i].X(), p[i].Y())
	}
}

func point(x int) bool {
	p[x] = Point{float64(rand.Intn(201) - 100), float64(rand.Intn(201) - 100)}
	for i := 0; i < x; i++ {
		if p[x].X() == p[i].X() && p[x].Y() == p[i].Y() {
			//fmt.Println("Not unique")
			return false
		}
	}
	return true
}

var p Path

func main() {
	if len(os.Args) == 2 {
		i, err := strconv.Atoi(os.Args[1])
		if err != nil {
			fmt.Println(err)
		} else {
			fmt.Printf("Generating a [%d] sides figure\n", i)
			fmt.Println("Figure's vertices")
			createSides(i)
			fmt.Println("Figure's Perimeter")
			p.Distance()
		}
	} else {
		fmt.Println("Please insert the number of sides")
	}
}
