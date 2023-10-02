mod data;
pub use data::*;

use std::ffi::{c_char, CStr, CString};
use tengwar::{mode::*, ToTengwar};


unsafe fn conv<M: Default + TengwarMode>(
    ptr: *const c_char,
    cfg: *const TengwarCfg,
) -> *mut c_char {
    let settings = cfg.as_ref().copied()
        .map(Into::into)
        .unwrap_or_default();

    let string = CStr::from_ptr(ptr)
        .to_string_lossy()
        .to_tengwar_with::<M>(settings);

    CString::new(string).unwrap().into_raw()
}


#[no_mangle]
pub extern "C" fn tw_config() -> TengwarCfg { TengwarCfg::new() }


#[no_mangle]
pub unsafe extern "C" fn tw_conv_beleriand(
    ptr: *const c_char,
    cfg: *const TengwarCfg,
) -> *mut c_char {
    conv::<Beleriand>(ptr, cfg)
}

#[no_mangle]
pub unsafe extern "C" fn tw_conv_classical(
    ptr: *const c_char,
    cfg: *const TengwarCfg,
) -> *mut c_char {
    conv::<Quenya>(ptr, cfg)
}

#[no_mangle]
pub unsafe extern "C" fn tw_conv_gondor(
    ptr: *const c_char,
    cfg: *const TengwarCfg,
) -> *mut c_char {
    conv::<Gondor>(ptr, cfg)
}


#[no_mangle]
pub unsafe extern "C" fn tw_free(ptr: *mut c_char) {
    if !ptr.is_null() {
        let _ = CString::from_raw(ptr);
    }
}
