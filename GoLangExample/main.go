// I watched a whole 2 hour video to learn Go just for this.

package main

import "C"
import "unsafe"

type FName struct
{
	ComparisonIndex uint;
	Number uint;
}

type UObject struct
{
	VFTable *uintptr
	ObjectFlags int
	InternalIndex int
	ClassPrivate *UObject
	NamePrivate FName
	OuterPrivate *UObject
}

func main() {
/*
You can do params like

params := struct {
	Map string
	ReturnValue *UObject
}{
	"Athena_Terrain"
}
*/

	var cmsg *C.char = C.CString("hi")
	// defer C.free(unsafe.Pointer(cmsg))
	fmt.Println("Hello!")
}
