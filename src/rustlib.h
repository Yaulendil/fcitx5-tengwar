#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

/// The type of behavior to be followed in the rendering of tehtar representing
///     "long" vowels.
enum class TengwarVowel {
  /// Always use the separate extended carrier mark.
  Separate = 0,
  /// Where possible, write the normal codepoint twice.
  Doubled = 2,
  /// Where possible, write the dedicated "long" codepoint once.
  Unique = 1,
};

struct TengwarCfg {
  /// If this is `true`, the A-tehta will be replaced with its alternate form.
  bool alt_a;
  /// If this is `true`, Sa-Rinci at the ends of words will use the alternate
  ///     form where appropriate.
  bool alt_rince;
  /// If this is `true`, the short carrier will be replaced by its ligating
  ///     variant where appropriate.
  bool ligate_short;
  /// If this is nonzero, zero-width joiners will be placed between glyphs to
  ///     form font ligatures where appropriate.
  uint8_t ligate_zwj;
  /// If this is `true`, the characters Silmë and Essë will use their inverted
  ///     Nuquernë variants when holding a tehta.
  bool nuquerna;
  /// This defines the treatment of "long" vowels.
  TengwarVowel vowels;
};

extern "C" {

TengwarCfg tw_config();

char *tw_conv_beleriand(const char *ptr, const TengwarCfg *cfg);

char *tw_conv_classical(const char *ptr, const TengwarCfg *cfg);

char *tw_conv_gondor(const char *ptr, const TengwarCfg *cfg);

void tw_free(char *ptr);

} // extern "C"
