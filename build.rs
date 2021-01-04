use std::path::Path;

fn main() {
    let recast_path = Path::new("deps/recastnavigation/Detour/Include");

    cxx_build::bridge("src/lib.rs")
        .file("src/nav_mesh.cc")
        .include(recast_path)
        .flag_if_supported("-std=c++14")
        .compile("detour-rs");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/nav_mesh.cc");
    println!("cargo:rerun-if-changed=include/nav_mesh.h");

    println!("cargo:rerun-if-changed={}", "build.rs");

    println!("cargo:rustc-flags=-l dylib=stdc++");

    let dst = cmake::build("deps");

    // println!("cargo:rerun-if-changed={}", "build.rs");
    // let _ = print_dirs(Path::new("recast"));

    println!("dst {}", dst.display());

    println!("cargo:rustc-link-search=native={}/lib", dst.display());
    println!("cargo:rustc-link-lib=static=Detour");
    // println!("cargo:rustc-link-lib=static=RecastNavigation");
}
