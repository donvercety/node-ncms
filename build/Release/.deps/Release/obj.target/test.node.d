cmd_Release/obj.target/test.node := g++ -shared -pthread -rdynamic -m64  -Wl,-soname=test.node -o Release/obj.target/test.node -Wl,--start-group Release/obj.target/test/test.o -Wl,--end-group 
