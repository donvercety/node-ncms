#include <node.h>
#include <pthread.h>
#include <unistd.h>

namespace demo {

	using v8::Context;
	using v8::Exception;
	using v8::Function;
	using v8::FunctionCallbackInfo;
	using v8::FunctionTemplate;
	using v8::Isolate;
	using v8::Local;
	using v8::Number;
	using v8::Object;
	using v8::Persistent;
	using v8::String;
	using v8::Value;

	struct Data {
		Local<Number> num;
		Local<Function> cb;
	};	

	void* threadFun(void *args) {
		// Isolate *isolate = Isolate::GetCurrent(); // nestava
		// struct Data *Arguments = (struct Data *)args;


		Isolate* isolate = (Isolate *) args;

		sleep(5);
		printf("isolate %p\n", isolate);

		isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));

		// printf("%d\n", Arguments->num->Uint32Value());

		// Local<Function> cb = Local<Function>::Cast(Arguments->cb);

		// how many arguments will be returned
		// const unsigned argc = 1;
		// Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "hello world") };
		// cb->Call(Null(isolate), argc, argv);

		// delete Arguments;
		return NULL;
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

		Arguments->num = Number::New(isolate, args[0]->NumberValue());
		Arguments->cb  = Local<Function>::Cast(args[1]);

		const unsigned argc = 1;
		Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "hello world") };
		Arguments->cb->Call(Null(isolate), argc, argv);

		pthread_t tid;
		pthread_create(&tid, NULL, threadFun, isolate);
		// pthread_join(tid, NULL);
	}

	void Init(Local<Object> exports, Local<Object> module) {
		NODE_SET_METHOD(module, "exports", RunCallback);
	}

	NODE_MODULE(test, Init)

}