#pragma once
#include "rust/cxx.h"

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

#include <memory>

class NavMesh {
  dtNavMesh* mesh;
  dtNavMeshQuery* query;

public:
  NavMesh(dtNavMeshParams* params);

  void add_tile(const rust::Slice<const unsigned char> data, int data_size, unsigned int tile_ref) const;
  rust::Vec<float> find_path(const std::array<float, 3> start_r, const std::array<float, 3> end_r, const std::array<float, 3> extents_r) const;
};

std::unique_ptr<NavMesh> create_nav_mesh(float orig_x, float orig_y, float orig_z, float tile_width, float tile_height, int max_tiles, int max_polys);