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


void TengwarState::set_candidate(int idx) {
	auto &panel = ic_->inputPanel();

	if (auto candidateList = panel.candidateList()) {
		auto &list1 = *candidateList;
		auto &list2 = static_cast<TengwarCandidateList&>(list1);
		list2.setCandidate(idx);
	}
}


void TengwarState::set_mode(TengwarMode mode) { mode_ = mode; }


void TengwarState::keyEvent(fcitx::KeyEvent &event) {
	fcitx::GlobalConfig &cfg = engine_->instance()->globalConfig();
	fcitx::Key key = event.key();
	auto &panel = ic_->inputPanel();

	if (auto candidateList = panel.candidateList()) {
		auto &list1 = *candidateList;
		auto &list2 = static_cast<TengwarCandidateList&>(list1);

		if (
			key.check(FcitxKey_Up)
			|| key.checkKeyList(cfg.defaultPrevCandidate())
		) {
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
	
		if (
			key.check(FcitxKey_Down)
			|| key.checkKeyList(cfg.defaultNextCandidate())
		) {
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

		if (key.check(FcitxKey_space)) {
			ic_->commitString(read_tengwar());

			reset();
			return;
		}

		if (key.check(FcitxKey_Escape)) {
			reset();
			return event.filterAndAccept();
		}

		//  MODE SWITCHING:
		if (key.check(FcitxKey_F1)) {
			mode_ = TengwarMode::Classical;
			updateUI();
			set_candidate(0);
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F2)) {
			mode_ = TengwarMode::Gondor;
			updateUI();
			set_candidate(1);
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F3)) {
			mode_ = TengwarMode::Beleriand;
			updateUI();
			set_candidate(2);
			return event.filterAndAccept();
		}
		// if (key.check(FcitxKey_F4)) {
		// 	mode_ = TengwarMode::General;
		//
		// 	updateUI();
		// 	set_candidate(3);
		// 	return event.filterAndAccept();
		// }

		//  CONFIG OPTIONS:
		if (key.check(FcitxKey_F5)) {
			engine_->cycle_vowel();
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F6)) {
			engine_->config_.nuquerna = !engine_->config_.nuquerna;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F7)) {
			engine_->config_.alt_rince = !engine_->config_.alt_rince;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F8)) {
			engine_->config_.alt_a = !engine_->config_.alt_a;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F9)) {
			engine_->config_.elide_a = !engine_->config_.elide_a;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F10)) {
			engine_->config_.dot_plain = !engine_->config_.dot_plain;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F11)) {
			engine_->config_.keep_a_init = !engine_->config_.keep_a_init;
			updateUI();
			return event.filterAndAccept();
		}
		if (key.check(FcitxKey_F12)) {
			engine_->config_.keep_a_long = !engine_->config_.keep_a_long;
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

		} else {
			aux_text += " [" + current_mode() + "]";
			pre_text = read_tengwar();
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
