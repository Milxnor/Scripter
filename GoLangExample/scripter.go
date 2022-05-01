package scripter

struct FName 
{
	uint ComparisonIndex;
	uint Number;
}

struct UObject
{
	uintptr* VFTable; // double ptr but whatever
	int ObjectFlags;
	int InternalIndex;
	UObject* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;
}