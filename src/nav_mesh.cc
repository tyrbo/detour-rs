#include <DetourNavMesh.h>
#include <DetourCommon.h>
#include <DetourNavMeshQuery.h>

#include "detour-rs/include/nav_mesh.h"

#include <cstring>
#include <iostream>
#include <memory>

std::unique_ptr<NavMesh> create_nav_mesh(float orig_x, float orig_y, float orig_z, float tile_width, float tile_height, int max_tiles, int max_polys) {
    dtNavMeshParams* params = new dtNavMeshParams();
    params->orig[0] = orig_x;
    params->orig[1] = orig_y;
    params->orig[2] = orig_z;
    params->tileWidth = tile_width;
    params->tileHeight = tile_height;
    params->maxTiles = max_tiles;
    params->maxPolys = max_polys;

    NavMesh* nav_mesh = new NavMesh(params);

    return std::unique_ptr<NavMesh>(nav_mesh);
}

// TODO: Handle return statuses

NavMesh::NavMesh(dtNavMeshParams* params) {
    this->mesh = dtAllocNavMesh();
    this->mesh->init(params);

    this->query = dtAllocNavMeshQuery();
}

void NavMesh::add_tile(const rust::Slice<const unsigned char> data, int data_size, unsigned int tile_ref) const {
    unsigned char* inner = (unsigned char*)dtAlloc(data_size, DT_ALLOC_PERM);
    memset(inner, 0, data_size);
    memcpy(inner, data.data(), data_size);

    this->mesh->addTile(inner, data_size, DT_TILE_FREE_DATA, tile_ref, 0);
}

rust::Vec<float> NavMesh::find_path(const std::array<float, 3> start_r, const std::array<float, 3> end_r, const std::array<float, 3> extents_r) const {
    const float* extents = extents_r.data();

    const float* start = start_r.data();
    const float* end = end_r.data();

    dtQueryFilter filter;
    dtPolyRef start_ref;
    dtPolyRef end_ref;

    this->query->init(this->mesh, 4092);

    this->query->findNearestPoly(start, extents, &filter, &start_ref, 0);
    this->query->findNearestPoly(end, extents, &filter, &end_ref, 0);

    if (!start_ref || !end_ref) {
        return rust::Vec<float>();
    }

    int npoly = 0;
    dtPolyRef path[1024] = { 0 };
    auto status = this->query->findPath(start_ref, end_ref, start, end, &filter, path, &npoly, 1024);

    if (npoly) {
        if (path[npoly - 1] != end_ref) {
            // float* pos = const_cast<float*>(start);

        	// this->query->closestPointOnPoly(path[npoly - 1], end, pos, 0);

            return rust::Vec<float>();
        }
    
        float straight_path[2048 * 3];
        unsigned char straight_path_flags[2048];
    
        int n_straight_polys;
        dtPolyRef straight_path_polys[2048];

        status = this->query->findStraightPath(start, end, path, npoly,
            straight_path, straight_path_flags,
            straight_path_polys, &n_straight_polys, 2048, DT_STRAIGHTPATH_AREA_CROSSINGS);

        if (dtStatusFailed(status)) {
            return rust::Vec<float>();
        }

        if (n_straight_polys) {
            rust::Vec<float> path;

            for (int i = 0; i < n_straight_polys; ++i)
            {
                path.push_back(straight_path[i * 3]);
                path.push_back(straight_path[i * 3 + 1]);
                path.push_back(straight_path[i * 3 + 2]);
            }

            return path;
        }
    }

    return rust::Vec<float>();
}