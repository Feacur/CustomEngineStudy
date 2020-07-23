local hello_string = "Hello, Lua!"
print(hello_string .. " (of lenght: " .. string.len(hello_string) .. ")")

function CustomAdd(a, b)
	print("[Lua] 'CustomAdd(" .. a .. ", " .. b .. ")'")
	return a + b
end

function CallNativeCustomAdd(a, b)
	print("[Lua] 'CallNativeCustomAdd(" .. a .. ", " .. b .. ")'")
	print("[Lua] its result is '" .. NativeCustomAdd(a, b) .. "'")
end
