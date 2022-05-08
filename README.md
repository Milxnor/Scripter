# Scripter
A dll that allows you to easily mod Fortnite in popular coding languages.<br>
Tested on 6.10 and 20.20, should work for <b>every</b> version.<br>

# Supported languages

C#, JS, C++

# TODO

- Add checks to make sure the user has the language the script is made in.<br>
- Add lua using this http://gamedevgeek.com/tutorials/calling-c-functions-from-lua/.<br>
- Add GoLang using cgo (https://stackoverflow.com/questions/8231618/how-to-import-a-dll-function-written-in-c-using-go).<br>
go build -o helloworld.dll -buildmode=c-shared
- Figure out how to add rust. The issue is that it can only call C.<br>
- Make deleting easier/automatic.<br>
- Add Java using https://stackoverflow.com/questions/55710599/how-to-load-access-and-use-c-dll-functions-in-java, the issue is that my Java knowledge isn't great and I don't know how to build as a DLL.

# FIX SOON

- C# doesn't work anymore.

# Tutorial

https://milxnor.gitbook.io/scripter/
