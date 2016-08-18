#include <stdio.h>  // printf
#include <stdlib.h> // malloc, free
#include <string.h> // strlen

#include <node.h>
#include <node_buffer.h>
#include <uv.h>
#include <unistd.h>

namespace ncms {

	using namespace v8;
	using namespace node;

	uv_loop_t *loop;

	// data to be available in different threads
	struct Data {
		char* buff;
		size_t size;
		Local<Function> cb;
	};

	void threadMaster(uv_work_t *req) {
		struct Data *Arguments = (struct Data *)req->data;

		printf("Processing in 2nd thread!\n");

		size_t l = Arguments->size;
		char* r  = (char*)malloc((l + 1) * sizeof(char));

		// do some heavy CPU intensive work
		sleep(5);

		int i;
		for(i = 0; i < l; i++) {
			r[i] = Arguments->buff[l - 1 - i];
		}

		strcpy(Arguments->buff, r);

		// print the buffer data when it is in 2nd thread
		printf("processed Buffer in 2nd thread: [%.*s]\n", Arguments->size, Arguments->buff);

		free(r);
	}

	void finish(uv_work_t *req, int status) {
		printf("Threaded Execution Finished!\n");

		Isolate *isolate = Isolate::GetCurrent();

		struct Data *Arguments = (struct Data *) req->data;

		// number of arguments to be returned
		const unsigned argc = 1;

		// how to return string value
		// ===========================
		// Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "hello world") };

		// how to return number value
		// ==========================
		// Local<Value> argv[argc] = { Number::New(isolate, 666) };

		// how to return buffer value
		// ==========================
		Local<Value> argv[argc] = { ArrayBuffer::New(isolate, Arguments->buff, Arguments->size) };

		Arguments->cb->Call(Null(isolate), argc, argv);
		delete Arguments;
	}

	void jsCallback(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		// check the number of arguments passed.
		if (args.Length() < 2) {

			// Throw an Error that is passed back to JavaScript
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		// check the argument types
		if (!args[0]->IsObject() || !args[1]->IsFunction()) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong arguments")));
			return;
		}

		struct Data *Arguments  = new Data();

		Local<Object> bufferObj = args[0]->ToObject();

		Arguments->buff = Buffer::Data(bufferObj);
		Arguments->size = Buffer::Length(bufferObj);
		Arguments->cb   = Local<Function>::Cast(args[1]);

		loop = uv_default_loop();
		uv_work_t req;

		req.data = Arguments;

		uv_queue_work(loop, &req, threadMaster, finish);	
		uv_run(loop, UV_RUN_DEFAULT);
	}

	void Init(Local<Object> exports, Local<Object> module) {
		NODE_SET_METHOD(module, "exports", jsCallback);
	}

	NODE_MODULE(thread, Init)

}