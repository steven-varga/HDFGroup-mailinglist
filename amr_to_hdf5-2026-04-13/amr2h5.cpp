/* copyright steven varga, vargalabs 2026, apr 13, Toronto, ON, Canada;  MIT license */

#include <cstdint>
#include <cmath>
#include <array>
#include <string>
#include <vector>
#include <h5cpp/core>
    #include "type.hpp"
#include <h5cpp/io>

/** @brief one AMR block with scalar and vector fields */
struct block_t {
    std::array<double, 3> origin, spacing;   /*!< x0, y0, z0 and dx, dy, dz */
    std::array<std::size_t, 3> dims;         /*!< I, J, K cell dimensions */
    std::uint32_t level;                     /*!< refinement level */
    std::vector<double> density, velocity;   /*!< size = I*J*K , size = I*J*K*3 */
};

/** @brief flatten shape helper */
inline auto num_cells(const std::array<std::size_t,3>& dims) -> std::size_t {
    return dims[0] * dims[1] * dims[2];
}
inline auto idx3(std::size_t i, std::size_t j, std::size_t k, std::size_t i_dim, std::size_t j_dim, std::size_t k_dim) -> std::size_t {
    return (i * j_dim + j) * k_dim + k;
}
inline auto idx4(std::size_t i, std::size_t j, std::size_t k, std::size_t c, std::size_t i_dim, std::size_t j_dim, std::size_t k_dim) -> std::size_t {
    return ((i * j_dim + j) * k_dim + k) * 3 + c;
}

inline block_t make_block(std::array<double,3> origin, std::array<double,3> spacing, std::array<std::size_t,3> dims, std::uint32_t level) {
    block_t b{ .origin = origin, .spacing = spacing, .dims = dims, .level = level,
        .density = std::vector<double>(num_cells(dims)), .velocity = std::vector<double>(num_cells(dims) * 3)};
    std::size_t i_dim = dims[0], j_dim = dims[1], k_dim = dims[2];

    for (std::size_t i = 0; i < i_dim; ++i) for (std::size_t j = 0; j < j_dim; ++j) for (std::size_t k = 0; k < k_dim; ++k) {
        const double x = origin[0] + (static_cast<double>(i) + 0.5) * spacing[0];
        const double y = origin[1] + (static_cast<double>(j) + 0.5) * spacing[1];
        const double z = origin[2] + (static_cast<double>(k) + 0.5) * spacing[2];

        const double r2 = (x - 0.5)*(x - 0.5) + (y - 0.5)*(y - 0.5) + (z - 0.5)*(z - 0.5);
        b.density[idx3(i,j,k,i_dim,j_dim,k_dim)] = 1.0 + 0.5 * std::exp(-40.0 * r2) + 0.1 * level;

        b.velocity[idx4(i,j,k,0,i_dim,j_dim,k_dim)] = -y;
        b.velocity[idx4(i,j,k,1,i_dim,j_dim,k_dim)] =  x;
        b.velocity[idx4(i,j,k,2,i_dim,j_dim,k_dim)] =  0.25 * std::sin(6.28318530718 * z);
    }
    return b;
}

inline auto mock() -> std::vector<block_t> {
    std::vector<block_t> blocks;

    /* coarse blocks */
    blocks.push_back(make_block({0.0, 0.0, 0.0}, {0.0625, 0.0625, 0.0625}, {8, 8, 8}, 0));
    blocks.push_back(make_block({0.5, 0.0, 0.0}, {0.0625, 0.0625, 0.0625}, {8, 8, 8}, 0));
    blocks.push_back(make_block({0.0, 0.5, 0.0}, {0.0625, 0.0625, 0.0625}, {8, 8, 8}, 0));

    /* finer patches */
    blocks.push_back(make_block({0.25, 0.25, 0.25}, {0.03125, 0.03125, 0.03125}, {8, 12, 10}, 1));
    blocks.push_back(make_block({0.55, 0.20, 0.35}, {0.015625, 0.015625, 0.015625}, {10, 9, 11}, 2));

    return blocks;
}

int main(){
    std::string h5_path = "amr_synthetic.h5";
    const std::vector<block_t>& blocks = mock();
    std::vector<std::array<double,3>> block_origins, block_spacings;
    std::vector<std::array<std::uint64_t,3>> block_dims;
    std::vector<std::uint32_t> block_levels;

    block_origins.reserve(blocks.size());
    block_spacings.reserve(blocks.size());
    block_dims.reserve(blocks.size());
    block_levels.reserve(blocks.size());

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const auto& b = blocks[i];
        block_origins.push_back(b.origin);
        block_spacings.push_back(b.spacing);
        block_dims.push_back({
            static_cast<std::uint64_t>(b.dims[0]),
            static_cast<std::uint64_t>(b.dims[1]),
            static_cast<std::uint64_t>(b.dims[2])
        });
        block_levels.push_back(b.level);
    }

    h5::fd_t fd = h5::create(h5_path, H5F_ACC_TRUNC);

    /* metadata */
    h5::write(fd, "/metadata/block_origins",  block_origins);
    h5::write(fd, "/metadata/block_spacings", block_spacings);
    h5::write(fd, "/metadata/block_dims", block_dims);
    h5::write(fd, "/metadata/block_levels", block_levels);

    /* fields: one group per block */
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const auto& b = blocks[i];
        const auto tag = std::format("/fields/block_{:04d}", i);

        const std::size_t i_dim = b.dims[0];
        const std::size_t j_dim = b.dims[1];
        const std::size_t k_dim = b.dims[2];

        if (b.density.size() != i_dim * j_dim * k_dim) throw std::runtime_error("density size mismatch");
        if (b.velocity.size() != i_dim * j_dim * k_dim * 3) throw std::runtime_error("velocity size mismatch");

        /* scalar field [I,J,K] */
        h5::write(fd, tag + "/density", b.density, h5::current_dims{i_dim, j_dim, k_dim},
            h5::chunk{std::min<std::size_t>(i_dim, 32), std::min<std::size_t>(j_dim, 32), std::min<std::size_t>(k_dim, 32)} | h5::gzip{4});

        /* vector field [I,J,K,3] */
        h5::write(fd, tag + "/velocity", b.velocity,
            h5::current_dims{i_dim, j_dim, k_dim, 3},
            h5::chunk{std::min<std::size_t>(i_dim, 16), std::min<std::size_t>(j_dim, 16), std::min<std::size_t>(k_dim, 16), 3} | h5::gzip{4});
    }
}