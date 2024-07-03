package main

import "fmt"

func reverse(a []int,size int)  {
	for i := 0; i < size/2; i++ {
		a[i],a[size-1-i]=a[size-1-i],a[i]
	}
}
func main() {
	var n,size int
	fmt.Scan(&n,&size)
	var nums =make([]int,n)
	for i := 0; i < n; i++ {
		fmt.Scan(&nums[i])
	}
	reverse(nums,size)
	for _, v := range nums {
		fmt.Printf("%d ", v)
	}
	fmt.Println()
}