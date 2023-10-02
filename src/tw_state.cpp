#include "tengwar.h"
#include "tw_panel.cpp"


std::string TengwarState::current_mode() const {
	return engine_->mode_label(mode_);
}


std::string TengwarState::read_tengwar(TengwarMode mode) const {
	std::string text_in = buffer_.userInput();
	std::string text_tw = engine_->convert(text_in, mode);
	return text_tw;
}


void TengwarState::set_mode(TengwarMode mode) { mode_ = mode; }


void TengwarState::keyEvent(fcitx::KeyEvent &event) {
	fcitx::GlobalConfig &cfg = engine_->instance()->globalConfig();
	fcitx::Key key = event.key();
	auto &panel = ic_->inputPanel();

	if (auto candidateList = panel.candidateList()) {
		auto &list1 = *candidateList;
		auto &list2 = static_cast<TengwarCandidateList&>(list1);

		// int idx = key.keyListIndex(selectionKeys);
		// if (0 <= idx && idx < candidateList->size()) {
		// 	event.accept();
		// 	candidateList->candidate(idx).select(ic_);
		// 	return;
		// }

		if (key.checkKeyList(cfg.defaultPrevCandidate())) {
			if (auto *movable = candidateList->toCursorMovable()) {
				event.accept();
				movable->prevCandidate();
				ic_->updateUserInterface(
					fcitx::UserInterfaceComponent::InputPanel
				);
			}

			updateText();
			return event.filterAndAccept();
		}
	
		if (key.checkKeyList(cfg.defaultNextCandidate())) {
			if (auto *movable = candidateList->toCursorMovable()) {
				event.accept();
				movable->nextCandidate();
				ic_->updateUserInterface(
					fcitx::UserInterfaceComponent::InputPanel
				);
			}

			updateText();
			return event.filterAndAccept();
		}

		if (0 <= candidateList->cursorIndex()) {
			if (key.check(FcitxKey_Return)) {
				list2.confirm();

				reset();
				return event.filterAndAccept();
			}

			if (key.check(FcitxKey_Escape)) {
				list2.clearCandidate();
				updateUI();
				return event.filterAndAccept();
			}
		}
	}

	if (!active()) {
		if (key.check(FcitxKey_space)) return;
		if (key.check(FcitxKey_Tab)) return;
		if (!key.isSimple()) return;
	} else {
		if (key.check(FcitxKey_BackSpace)) {
			buffer_.backspace();

			updateUI();
			return event.filterAndAccept();
		}

		if (key.check(FcitxKey_Return)) {
			ic_->commitString(read_tengwar());

			reset();
			return event.filterAndAccept();
		}

		if (key.check(FcitxKey_Escape)) {
			reset();
			return event.filterAndAccept();
		}

		//  MODE SWITCHING:
		if (key.check(FcitxKey_F5)) {
			preview_ = false;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F6)) {
			mode_ = TengwarMode::Classical;
			preview_ = true;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F7)) {
			mode_ = TengwarMode::Gondor;
			preview_ = true;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F8)) {
			mode_ = TengwarMode::Beleriand;
			preview_ = true;

			updateUI();
			return event.filterAndAccept();
		}

		//  CONFIG OPTIONS:
		if (key.check(FcitxKey_F9)) {
			// engine_->config_.alt_a = !engine_->config_.alt_a;
			engine_->config_.nuquerna = !engine_->config_.nuquerna;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F10)) {
			engine_->config_.alt_rince = !engine_->config_.alt_rince;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F11)) {
			engine_->config_.ligate_short = !engine_->config_.ligate_short;

			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F12)) {
			// engine_->config_.cycle_vowel();
			engine_->config_.ligate_zwj += 1;
			engine_->config_.ligate_zwj %= 4;
		
			updateUI();
			return event.filterAndAccept();
		}
	}

	if (key.isSimple()) {
		buffer_.type(key.sym());
		updateUI();
	}

	return event.filterAndAccept();
}


void TengwarState::updateText() {
	if (active()) {
		fcitx::InputPanel &inputPanel = ic_->inputPanel();
		auto &list1 = *inputPanel.candidateList();
		auto &list2 = static_cast<TengwarCandidateList&>(list1);
		int idx = list2.cursorIndex();

		std::string input = buffer_.userInput();
		std::string aux_text = input;
		std::string pre_text;

		if (0 <= idx) {
			auto &current = list2.current();
			aux_text += " [" + current.tw_mode_label() + "]";
			pre_text = current.tw_text();

		} else if (preview_) {
			aux_text += " [" + current_mode() + "]";
			pre_text = read_tengwar();

		} else {
			pre_text = input;
		}

		if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit)) {
			fcitx::Text preedit(pre_text, fcitx::TextFormatFlag::HighLight);
			inputPanel.setClientPreedit(preedit);
		} else {
			fcitx::Text preedit(pre_text);
			inputPanel.setPreedit(preedit);
		}

		inputPanel.setAuxUp(fcitx::Text(aux_text));
	}

	ic_->updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
	ic_->updatePreedit();
}


void TengwarState::updateUI() {
	fcitx::InputPanel &inputPanel = ic_->inputPanel();
	std::string input = buffer_.userInput();
	std::string pre_text;

	inputPanel.reset();

	if (active()) {
		inputPanel.setCandidateList(
			std::make_unique<TengwarCandidateList>(engine_, ic_, input)
		);
	}

	updateText();
}
