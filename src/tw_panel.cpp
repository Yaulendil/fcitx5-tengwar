#include <fcitx/inputpanel.h>
#include "tengwar.h"


namespace {
#define ENTRIES 3


class TengwarCandidateWord: public fcitx::CandidateWord {
public:
	TengwarCandidateWord(
		TengwarEngine *engine,
		std::string text,
		TengwarMode mode
	): engine_(engine), text_(text), mode_(mode) {
		setText(fcitx::Text(text));
	}

	void select(fcitx::InputContext *inputContext) const override {
		inputContext->commitString(text_);
		auto state = inputContext->propertyFor(engine_->factory());
		state->set_mode(mode_);
		state->reset();
	}

	std::string tw_text() const { return text_; }
	TengwarMode tw_mode() const { return mode_; }
	std::string tw_mode_label() const { return engine_->mode_label(mode_); }
private:
	TengwarEngine *engine_;
	std::string text_;
	TengwarMode mode_;
};


class TengwarCandidateList:
	public fcitx::CandidateList,
	public fcitx::CursorMovableCandidateList {
public:
	TengwarCandidateList(
		TengwarEngine *engine,
		fcitx::InputContext *ic,
		const std::string &text
	): engine_(engine), ic_(ic), text_(text) {
		setCursorMovable(this);
		generate();
	}

	fcitx::CandidateLayoutHint layoutHint() const override {
		return fcitx::CandidateLayoutHint::Vertical;
	}

	const fcitx::Text &label(int idx) const override {
		return labels_[idx];
	}

	const fcitx::CandidateWord &candidate(int idx) const override {
		return *candidates_[idx];
	}

	const TengwarCandidateWord &current() const {
		return *candidates_[cursor_];
	}

	void confirm() {
		int idx = cursorIndex();
		if (0 <= idx && idx < ENTRIES) {
			auto &selected = candidate(idx);
			selected.select(ic_);
		}
	}

	int size() const override { return ENTRIES; }
	int cursorIndex() const override { return cursor_; }
	void clearCandidate() { cursor_ = -1; }

	void moveCandidate(int diff) {
		cursor_ = (cursor_ + ENTRIES + diff) % ENTRIES;
	}

	void nextCandidate() override {
		moveCandidate(1);
	}

	void prevCandidate() override {
		if (cursor_ < 0) cursor_ = 0;
		moveCandidate(-1);
	}
private:
	void generate() {
		auto state = ic_->propertyFor(engine_->factory());

		candidates_[0] = std::make_unique<TengwarCandidateWord>(
			engine_, state->read_tengwar(TengwarMode::Classical),
			TengwarMode::Classical
		);
		candidates_[1] = std::make_unique<TengwarCandidateWord>(
			engine_, state->read_tengwar(TengwarMode::Gondor),
			TengwarMode::Gondor
		);
		candidates_[2] = std::make_unique<TengwarCandidateWord>(
			engine_, state->read_tengwar(TengwarMode::Beleriand),
			TengwarMode::Beleriand
		);
	}

	TengwarEngine *engine_;
	fcitx::InputContext *ic_;
	fcitx::Text labels_[ENTRIES] = {
		// fcitx::Text("Classical:"),
		// fcitx::Text("Gondor:"),
		// fcitx::Text("Beleriand:"),
		fcitx::Text("C."),
		fcitx::Text("G."),
		fcitx::Text("B."),
		// fcitx::Text("1."),
		// fcitx::Text("2."),
		// fcitx::Text("3."),
	};
	std::unique_ptr<TengwarCandidateWord> candidates_[ENTRIES];
	std::string text_;
	int cursor_ = -1;
};

} // namespace