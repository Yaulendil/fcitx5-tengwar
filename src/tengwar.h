#ifndef _FCITX5_TENGWAR_H_
#define _FCITX5_TENGWAR_H_

#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>
#include <fcitx-utils/inputbuffer.h>
#include <string>
#include "rustlib.h"


enum class TengwarMode {
    /// Classical Mode, devised by Fëanáro Finwion for Noldorin Quenya.
    Classical,
    /// Mode of Beleriand, adapted in the First Age for writing Sindarin.
    Beleriand,
    /// Mode of Gondor, used for Sindarin in the Third Age.
    Gondor,
};


class TengwarEngine;


class TengwarState: public fcitx::InputContextProperty {
public:
	TengwarState(
		TengwarEngine *engine,
		fcitx::InputContext *ic
	): engine_(engine), ic_(ic) {}

	std::string current_mode() const;
	std::string read_tengwar() const { return read_tengwar(mode_); }
	std::string read_tengwar(TengwarMode mode) const;

	bool active() const { return !buffer_.empty(); }
	void set_mode(TengwarMode mode);
	void keyEvent(fcitx::KeyEvent &keyEvent);
	void updateText();
	void updateUI();
	void reset() {
		buffer_.clear();
		updateUI();
	}
private:
	TengwarEngine *engine_;
	fcitx::InputContext *ic_;
	fcitx::InputBuffer buffer_{{
		fcitx::InputBufferOption::NoOption
	}};

	TengwarMode mode_ = TengwarMode::Classical;
	bool preview_ = true;
};


class TengwarEngine: public fcitx::InputMethodEngineV2 {
public:
	TengwarEngine(fcitx::Instance *instance);

	void keyEvent(
		const fcitx::InputMethodEntry &entry,
		fcitx::KeyEvent &event
	) override;

	void reset(
		const fcitx::InputMethodEntry &entry,
		fcitx::InputContextEvent &event
	) override;

	auto factory() const { return &factory_; }
	auto instance() const { return instance_; }
	std::string convert(std::string input, TengwarMode mode) const;
	std::string mode_label(TengwarMode mode) const;

private:
	fcitx::Instance *instance_;
	fcitx::FactoryFor<TengwarState> factory_;
public:
	TengwarCfg config_ = tw_config();
};


class TengwarEngineFactory: public fcitx::AddonFactory {
	fcitx::AddonInstance *create(fcitx::AddonManager* manager) override {
		FCITX_UNUSED(manager);
		return new TengwarEngine(manager->instance());
	}
};

#endif
