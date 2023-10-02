use tengwar::{TranscriberSettings, VowelStyle};


#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(C)]
pub struct TengwarCfg {
    /// If this is `true`, the A-tehta will be replaced with its alternate form.
    pub alt_a: bool,

    /// If this is `true`, Sa-Rinci at the ends of words will use the alternate
    ///     form where appropriate.
    pub alt_rince: bool,

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
