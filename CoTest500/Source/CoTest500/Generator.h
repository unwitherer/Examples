
#pragma once

#include "CoreMinimal.h"

#if __has_include(<coroutine>)
	#include <coroutine>
	namespace co = std;
#else
	#include <experimental/coroutine>
	namespace co = std::experimental;
#endif

struct FAwaiter
{
	const int32 n;
	// コルーチンの中断判定　falseで中断する
	constexpr bool await_ready() const
	{
		// 3の倍数か3のつく数で中断する
		return !(n % 3 == 0 || FString::FromInt(n).Contains(TEXT("3")));
	}
	// await_readyで中断したときに評価される
	// この関数の戻り値がcoroutine_handle型なら、それをresumeする
	// この関数の戻り値がbool型の場合、評価結果がfalseなら再開する
	// それ以外は単に関数が評価される
	constexpr void await_suspend(co::coroutine_handle<>) const noexcept {}
	// await_readyで中断しなかったときや
	// コルーチン再開時に co_await 式の評価結果を返す
	constexpr void await_resume() const noexcept {}
};

struct FPromise;
class FGenerator
{
	friend FPromise;

	using FCoroutineHandle = co::coroutine_handle<FPromise>;
	FCoroutineHandle Handle; // FPromise型のコルーチンを制御するコルーチンハンドル

private:
	// PromiseからGeneratorを生成
	explicit FGenerator(FPromise& inPromise) :
		Handle(FCoroutineHandle::from_promise(inPromise)){}

	// デフォルト・コピーコンストラクタ, コピー代入演算子の削除
	FGenerator() = delete;
	FGenerator(const FGenerator&) = delete;
	FGenerator& operator = (const FGenerator&) = delete;

public:
	// ムーブコンストラクタ, ムーブ代入演算子はムーブ元を削除する
	FGenerator(FGenerator&& inGenerator) :
		Handle(std::exchange(inGenerator.Handle, nullptr)){}
	FGenerator& operator = (FGenerator&& inGenerator)
	{
		Handle = std::exchange(inGenerator.Handle, nullptr);
		return *this;
	}
	~FGenerator()
	{
		if (Handle)
			Handle.destroy();
	}
	void Resume()
	{
		if (Handle && !Handle.done())
			Handle.resume();
	}
};

struct FPromise
{
	// コルーチン初回呼び出し時 必ず中断する
	constexpr co::suspend_always initial_suspend() const { return {}; }
	// コルーチン終了時 必ず中断する
	constexpr co::suspend_always final_suspend() const noexcept { return {}; }
	// co_returnで終了時 または コルーチン終端到達時に実行される
	constexpr void return_void() const noexcept {}
	// 例外発生時 terminate()で終了する
	void unhandled_exception() { std::terminate(); }
	// co_awaitに指定した整数をFAwaiterに渡し、中断可能判定なら中断する
	constexpr FAwaiter await_transform(const int32 inValue) { return { inValue }; }

	// コルーチンの戻り値としてPromise型からジェネレーターを生成する
	FGenerator get_return_object()
	{
		return FGenerator(*this);
	}
};

template <class... ArgTypes>
struct co::coroutine_traits<FGenerator, ArgTypes...>
{
	using promise_type = FPromise;
};