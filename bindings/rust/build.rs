use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-search=native=../../build");
    println!("cargo:rustc-link-lib=dylib=skip");

    let bindings = bindgen::Builder::default()
        .header("../../skip.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .rustified_enum("SkipDataTypeCode")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
