/*The MIT License (MIT)

Copyright (c) 2016 BeatrixKiddo/PraetorOmega

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include "Observer.hpp"
#include "Signal.hpp"

namespace Event
{
	Observer::~Observer() {
		detach();
	}
	void Observer::detach() noexcept {
		for (auto& signal : _signals)
			signal->slotDetach(this);
		_signals.clear();
	}
	void Observer::detach(SignalAbstract* signal) noexcept {
		const auto& found = std::find(std::begin(_signals), std::end(_signals), signal);
		if (found != std::end(_signals)) {
			_signals.erase(found);
			signal->slotDetach(this);
		}
	}
	void Observer::signalAttach(SignalAbstract* signal) {
		_signals.emplace_back(signal);
	}
	void Observer::signalDetach(SignalAbstract* signal) noexcept {
		_signals.erase(std::remove(std::begin(_signals), std::end(_signals), signal), std::end(_signals));
	}
}
