#include "tengwar.h"
#include "tw_state.cpp"


TengwarEngine::TengwarEngine(fcitx::Instance *instance):
	instance_(instance),
	factory_([this](fcitx::InputContext &ic) {
		return new TengwarState(this, &ic);
	})
{
	instance->inputContextManager().registerProperty(
		"tengwarState",
		&factory_
	);
}

void TengwarEngine::keyEvent(
	const fcitx::InputMethodEntry &entry,
	fcitx::KeyEvent &event
) {
	FCITX_UNUSED(entry);
	// FCITX_INFO() << keyEvent.key();

	if (event.isRelease() || event.key().states()) return;

	auto ic = event.inputContext();
	auto *state = ic->propertyFor(&factory_);
	state->keyEvent(event);
}

void TengwarEngine::reset(
	const fcitx::InputMethodEntry &entry,
	fcitx::InputContextEvent &event
) {
	FCITX_UNUSED(entry);

	auto *state = event.inputContext()->propertyFor(&factory_);
	state->reset();
}

std::string TengwarEngine::convert(std::string input, TengwarMode mode) const {
    char *tengwar;

    switch (mode) {
        case TengwarMode::Classical:
        tengwar = tw_conv_classical(input.data(), &config_);
        break;

        case TengwarMode::Beleriand:
        tengwar = tw_conv_beleriand(input.data(), &config_);
        break;

        case TengwarMode::Gondor:
        tengwar = tw_conv_gondor(input.data(), &config_);
        break;
    }

    std::string clone(tengwar);
    tw_free(tengwar);
    return clone;
}


std::string TengwarEngine::mode_label(TengwarMode mode) const {
	switch (mode) {
		case TengwarMode::Classical: return "Classical Mode";
		case TengwarMode::Beleriand: return "Mode of Beleriand";
		case TengwarMode::Gondor:    return "Mode of Gondor";
	}

	return "Unknown Mode";
}

// // static constexpr char configFile[] = "conf/clipboard.conf";
// // void reloadConfig() override { readAsIni(config_, configFile); }
// // 
// // const Configuration *getConfig() const override { return &config_; }
// // void setConfig(const RawConfig &config) override {
// // 	config_.load(config, true);
// // 	safeSaveAsIni(config_, configFile);
// // }
// 
FCITX_ADDON_FACTORY(TengwarEngineFactory);
// FCITX_CONFIGURATION(
//     ClipboardConfig, fcitx::KeyListOption triggerKey{this,
//                                               "TriggerKey",
//                                               ("Trigger Key"),
//                                               {fcitx::Key("Control+semicolon")},
//                                               fcitx::KeyListConstrain()};
//     fcitx::KeyListOption pastePrimaryKey{
//         this, "PastePrimaryKey", ("Paste Primary"), {}, fcitx::KeyListConstrain()};
//     fcitx::Option<int, fcitx::IntConstrain> numOfEntries{this, "Number of entries",
//                                            ("Number of entries"), 5,
//                                            fcitx::IntConstrain(3, 30)};);
