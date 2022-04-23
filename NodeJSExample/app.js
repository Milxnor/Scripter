const ffi = require("ffi-napi")
const int64 = require('node-int64')

function TEXT(text) { // Convert JSString to CString
    return Buffer.from('${text}\0', "ucs2");
}

const Scripter = new ffi.Library("Scripter", {
    "FindObject": [
        "int64", ["string"]
    ]
});

console.log(Scripter.FindObject(TEXT("FortEngine_"))));