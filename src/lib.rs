pub use ffi::{NavMesh, create_nav_mesh};
pub use cxx::UniquePtr;

unsafe impl Send for ffi::NavMesh {}
unsafe impl Sync for ffi::NavMesh {}

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("detour-rs/include/nav_mesh.h");

        type NavMesh;

        fn create_nav_mesh(
            orig_x: f32,
            orig_y: f32,
            orig_z: f32,
            tile_width: f32,
            tile_height: f32,
            max_tiles: i32,
            max_polys: i32,
        ) -> UniquePtr<NavMesh>;

        fn add_tile(self: &NavMesh, data: &[u8], data_size: i32, tile_ref: u32);
        fn find_path(self: &NavMesh, start: [f32; 3], end: [f32; 3], extents: [f32; 3]) -> Vec<f32>;
    }
}