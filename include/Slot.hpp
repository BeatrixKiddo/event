/*The MIT License (MIT)

Copyright (c) 2016 BeatrixKiddo/PraetorOmega

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once
#include <functional>
#include <memory>

namespace Event
{
	template <typename Return, typename... Args> class Slot
	{
		struct Abstract
		{
			virtual Return callback(Args... args) = 0;
			virtual bool operator==(const Abstract& slot) const noexcept = 0;
			virtual Observer* observer() noexcept = 0;
		};
		template <typename Class, typename Method> struct Concrete : Abstract
		{
			Class* _observer;
			std::function<Return(Class, Args...)> _method;
			Concrete(Class* observer, Method method)
				: _observer(observer)
				, _method(method)
			{}
			virtual Return callback(Args... args) override final {
				return _method(*_observer, args...);
			}
			virtual bool operator==(const Abstract& slot) const noexcept override final {
				auto&& temp = static_cast<const Concrete<Class, Method>&>(slot);
				return (_observer == temp._observer) && (_method.target_type() == temp._method.target_type());
			}
			virtual Observer* observer() noexcept override final {
				return _observer;
			}
		};
		std::unique_ptr<Abstract> _slot;
	public:
		template <typename Class> Slot(Class* observer, Return(Class::*method)(Args...))
			: _slot(std::make_unique<Concrete<Class, decltype(method)>>(observer, method))
		{}
		Slot(Slot&& slot)
			: _slot(std::move(slot._slot))
		{}
		Slot& operator=(Slot&& slot) {
			if (this != &slot)
				_slot = std::move(slot._slot);
			return *this;
		}
		Return callback(Args... args) {
			return _slot->callback(args...);
		}
		bool operator==(const Slot& slot) const noexcept {
			return *(_slot.get()) == *(slot._slot.get());
		}
		Observer* observer() noexcept {
			return _slot->observer();
		}
	};
}

