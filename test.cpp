#include <node.h>
#include <uv.h>
#include <unistd.h>

namespace demo {

	using v8::Exception;
	using v8::Function;
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Number;
	using v8::Object;
	using v8::String;
	using v8::Value;

	uv_loop_t *loop;

	struct Data {
		Local<Number> num;
		Local<Function> cb;
	};

	void threadMaster(uv_work_t *req) {
		sleep(5);
	}

	void finish(uv_work_t *req, int status) {
		Isolate *isolate = Isolate::GetCurrent();

		struct Data *Arguments = (struct Data *)req->data;
		printf("FINISHED %p\n", isolate);


		const unsigned argc = 1;
		Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "hello world") };
		Arguments->cb->Call(Null(isolate), argc, argv);

		delete Arguments;
	}

	void RunCallback(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		// Check the number of arguments passed.
		if (args.Length() < 2) {

			// Throw an Error that is passed back to JavaScript
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		// Check the argument types
		if (!args[0]->IsNumber() || !args[1]->IsFunction()) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong arguments")));
			return;
		}

		struct Data *Arguments = new Data();

		Arguments->num  = Number::New(isolate, args[0]->NumberValue());
		Arguments->cb = Local<Function>::Cast(args[1]);

		loop = uv_default_loop();
		uv_work_t req;

		req.data = Arguments;

		uv_queue_work(loop, &req, threadMaster, finish);
		// uv_thread_join(&hare_id); // blocks async
		
		uv_run(loop, UV_RUN_DEFAULT);
	}

	void Init(Local<Object> exports, Local<Object> module) {
		NODE_SET_METHOD(module, "exports", RunCallback);
	}

	NODE_MODULE(test, Init)

}