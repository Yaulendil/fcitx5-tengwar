extern crate cbindgen;

use std::{env, path::Path};
use cbindgen::{Builder, Config};


fn main() {
    let crate_env: String = env::var("CARGO_MANIFEST_DIR").unwrap();
    let crate_dir: &Path = Path::new(&crate_env);
    let config: Config = Config::from_root_or_default(crate_dir);

    Builder::new().with_crate(crate_dir.to_str().unwrap())
        .with_config(config)
        .generate()
        .expect("Cannot generate header file!")
        .write_to_file("../src/rustlib.h");
}
