
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
	// �R���[�`���̒��f����@false�Œ��f����
	constexpr bool await_ready() const
	{
		// 3�̔{����3�̂����Œ��f����
		return !(n % 3 == 0 || FString::FromInt(n).Contains(TEXT("3")));
	}
	// await_ready�Œ��f�����Ƃ��ɕ]�������
	// ���̊֐��̖߂�l��coroutine_handle�^�Ȃ�A�����resume����
	// ���̊֐��̖߂�l��bool�^�̏ꍇ�A�]�����ʂ�false�Ȃ�ĊJ����
	// ����ȊO�͒P�Ɋ֐����]�������
	constexpr void await_suspend(co::coroutine_handle<>) const noexcept {}
	// await_ready�Œ��f���Ȃ������Ƃ���
	// �R���[�`���ĊJ���� co_await ���̕]�����ʂ�Ԃ�
	constexpr void await_resume() const noexcept {}
};

struct FPromise;
class FGenerator
{
	friend FPromise;

	using FCoroutineHandle = co::coroutine_handle<FPromise>;
	FCoroutineHandle Handle; // FPromise�^�̃R���[�`���𐧌䂷��R���[�`���n���h��

private:
	// Promise����Generator�𐶐�
	explicit FGenerator(FPromise& inPromise) :
		Handle(FCoroutineHandle::from_promise(inPromise)){}

	// �f�t�H���g�E�R�s�[�R���X�g���N�^, �R�s�[������Z�q�̍폜
	FGenerator() = delete;
	FGenerator(const FGenerator&) = delete;
	FGenerator& operator = (const FGenerator&) = delete;

public:
	// ���[�u�R���X�g���N�^, ���[�u������Z�q�̓��[�u�����폜����
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
	// �R���[�`������Ăяo���� �K�����f����
	constexpr co::suspend_always initial_suspend() const { return {}; }
	// �R���[�`���I���� �K�����f����
	constexpr co::suspend_always final_suspend() const noexcept { return {}; }
	// co_return�ŏI���� �܂��� �R���[�`���I�[���B���Ɏ��s�����
	constexpr void return_void() const noexcept {}
	// ��O������ terminate()�ŏI������
	void unhandled_exception() { std::terminate(); }
	// co_await�Ɏw�肵��������FAwaiter�ɓn���A���f�\����Ȃ璆�f����
	constexpr FAwaiter await_transform(const int32 inValue) { return { inValue }; }

	// �R���[�`���̖߂�l�Ƃ���Promise�^����W�F�l���[�^�[�𐶐�����
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