/*The MIT License (MIT)

Copyright (c) 2016 BeatrixKiddo/PraetorOmega

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once
#include <algorithm>
#include <type_traits>
#include <vector>
#include "Observer.hpp"
#include "Result.hpp"
#include "Slot.hpp"

namespace Event
{
	class SignalAbstract
	{
		friend class Observer;
		virtual void slotDetach(Observer* observer) noexcept = 0;
	public:
		virtual ~SignalAbstract() = default;
	};

	template <Results Policy, typename Return, typename... Args> class Signal
		: public SignalAbstract
	{
		std::vector<Slot<Return, Args...>> _slots;
		virtual void slotDetach(Observer* observer) noexcept override final {
			_slots.erase(std::remove_if(_slots.begin(), _slots.end(), [&](auto& slot) { return slot.observer() == observer; }), _slots.end());
		}
		template <typename Return_ = Return, std::enable_if_t<std::is_void<Return_>::value>* = nullptr>
		void callback(std::vector<Result<Return>>& results, Slot<Return, Args...>& slot, Args... args) {
			slot.callback(args...);
			results.emplace_back();
		}
		template <typename Return_ = Return, std::enable_if_t<!std::is_void<Return_>::value>* = nullptr>
		void callback(std::vector<Result<Return>>& results, Slot<Return, Args...>& slot, Args... args) {
			results.emplace_back(slot.callback(args...));
		}
	public:
		~Signal() {
			detach();
		}
		void detach(Observer* observer) noexcept {
			const auto& found = std::find_if(_slots.begin(), _slots.end(), [&](auto& slot) { return slot.observer() == observer; });
			if (found != _slots.end()) {
				_slots.erase(found);
				observer->signalDetach(this);
			}
		}
		void detach() noexcept {
			for (auto& slot : _slots)
				slot.observer()->signalDetach(this);
			_slots.clear();
		}
		template <typename Class>
		void attach(Class* observer, Return(Class::*method)(Args...)) {
			auto&& newSlot = Slot<Return, Args...>(observer, method);
			const auto& found = std::find_if(_slots.begin(), _slots.end(), [&](const auto& slot) { return slot == newSlot; });
			if (found == _slots.end()) {
				_slots.emplace_back(std::move(newSlot));
				observer->signalAttach(this);
			}
		}
		template <Results Policy_ = Policy, std::enable_if_t<Policy_ == Results::Return>* = nullptr>
		std::vector<Result<Return>> operator()(Args... args) {
			auto&& results = std::vector<Result<Return>>();
			for (auto& slot : _slots) {
				try {
					callback(results, slot, args...);
				} catch(const std::exception& error) {
					results.emplace_back(error);
				}
			}
			return results;
		}
		template <Results Policy_ = Policy, std::enable_if_t<Policy_ == Results::Ignore>* = nullptr>
		void operator()(Args... args) noexcept {
			for (auto& slot : _slots) {
				try {
					slot.callback(args...);
				} catch(const std::exception& error) {
				}
			}
		}
	};
}

