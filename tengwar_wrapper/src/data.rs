use tengwar::{TranscriberSettings, VowelStyle};


#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(C)]
pub struct TengwarCfg {
    /// If this is `true`, the A-tehta will be replaced with its alternate form.
    pub alt_a: bool,

    /// If this is `true`, Sa-Rinci at the ends of words will use the alternate
    ///     form where appropriate.
    pub alt_rince: bool,

    /// If this is `true`, any tengwa that does not carry a tehta will be marked
    ///     with a dot.
    pub dot_plain: bool,

    /// If this is `true`, the A-tehta will not be used.
    pub elide_a: bool,

    /// If this is `true`, the A-tehta will not be elided initially.
    pub keep_a_init: bool,

    /// If this is `true`, the A-tehta will not be elided when long.
    pub keep_a_long: bool,

    /// If this is `true`, the short carrier will be replaced by its ligating
    ///     variant where appropriate.
    pub ligate_short: bool,

    /// If this is nonzero, zero-width joiners will be placed between glyphs to
    ///     form font ligatures where appropriate.
    pub ligate_zwj: u8,

    /// If this is `true`, the characters Silmë and Essë will use their inverted
    ///     Nuquernë variants when holding a tehta.
    pub nuquerna: bool,

    /// This defines the treatment of "long" vowels.
    pub vowels: TengwarVowel,
}

impl TengwarCfg {
    pub const fn new() -> Self {
        Self {
            alt_a: false,
            alt_rince: true,
            dot_plain: false,
            elide_a: false,
            keep_a_init: false,
            keep_a_long: false,
            ligate_short: false,
            ligate_zwj: 0,
            nuquerna: true,
            vowels: TengwarVowel::Doubled,
        }
    }
}

impl From<TengwarCfg> for TranscriberSettings {
    fn from(value: TengwarCfg) -> Self {
        Self {
            alt_a: value.alt_a,
            alt_rince: value.alt_rince,
            dot_plain: value.dot_plain,
            elide_a: value.elide_a,
            keep_a_init: value.keep_a_init,
            keep_a_long: value.keep_a_long,
            ligate_short: value.ligate_short,
            ligate_zwj: value.ligate_zwj,
            nuquerna: value.nuquerna,
            vowels: value.vowels.into(),
        }
    }
}


/// The type of behavior to be followed in the rendering of tehtar representing
///     "long" vowels.
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(C)]
pub enum TengwarVowel {
    /// Always use the separate extended carrier mark.
    Separate = 0,
    /// Where possible, write the normal codepoint twice.
    Doubled = 2,
    /// Where possible, write the dedicated "long" codepoint once.
    Unique = 1,
}

impl From<TengwarVowel> for VowelStyle {
    fn from(value: TengwarVowel) -> Self {
        match value {
            TengwarVowel::Separate => Self::Separate,
            TengwarVowel::Doubled => Self::Doubled,
            TengwarVowel::Unique => Self::Unique,
        }
    }
}


#[no_mangle]
pub extern "C" fn tw_vowel_cycle(vowel: TengwarVowel) -> TengwarVowel {
    match vowel {
        TengwarVowel::Separate => TengwarVowel::Doubled,
        TengwarVowel::Doubled => TengwarVowel::Unique,
        TengwarVowel::Unique => TengwarVowel::Separate,
    }
}
