#include "../include/packet.hpp"

std::size_t bind_socket(std::uint32_t *fd, struct sockaddr_in *addr) {
  
  return (0);
}

/**
 * @brief
 *
 * @param fd
 * @param addr
 * @param msg
 * @return std::size_t
 */
std::size_t send_heartbeat(std::uint32_t fd, struct sockaddr_in *addr,
                           std::string msg) {
  std::size_t status = sendto(fd, msg.c_str(), msg.size(), 0,
                              (struct sockaddr *)addr, sizeof(struct sockaddr));
  if (status != msg.size()) {
    spdlog::error("send failed!");
    spdlog::error(std::to_string(status).c_str());
  } else {
    spdlog::debug("sent socket heartbeat");
  }
  return (status);
}

/**
 * @brief Parses the Initial Value (IV), or Nonce, from the received packet.
 *
 * @param nonce an empty 8-byte string
 * @param bytes a unsigned char array that contains at least 0x44 bytes, with
 the IV values located at 0x40
 */
void parse_nonce(unsigned char *nonce, unsigned char *bytes) {
  std::uint32_t iv1;
  std::uint32_t iv2;

  memcpy(&iv1, &bytes[0x40], 4);
  iv2 = iv1 ^ 0xDEADBEAF;

  memcpy(&nonce[0], &iv2, 4);
  memcpy(&nonce[4], &iv1, 4);

  return;
}

GT7Packet parse_bytes(unsigned char *bytes, std::uint32_t bytes_len) {
  GT7Packet packet;

  memcpy(&packet.position[0], &bytes[0x04], sizeof packet.position[0]);
  memcpy(&packet.position[1], &bytes[0x08], sizeof packet.position[1]);
  memcpy(&packet.position[2], &bytes[0x0C], sizeof packet.position[2]);

  memcpy(&packet.velocity[0], &bytes[0x10], sizeof packet.velocity[0]);
  memcpy(&packet.velocity[1], &bytes[0x14], sizeof packet.velocity[1]);
  memcpy(&packet.velocity[2], &bytes[0x1A], sizeof packet.velocity[2]);

  memcpy(&packet.rotation_im_i, &bytes[0x1C], sizeof packet.rotation_im_i);
  memcpy(&packet.rotation_im_j, &bytes[0x20], sizeof packet.rotation_im_j);
  memcpy(&packet.rotation_im_k, &bytes[0x24], sizeof packet.rotation_im_k);
  memcpy(&packet.rotation_real, &bytes[0x28], sizeof packet.rotation_real);

  memcpy(&packet.angular_velocity[0], &bytes[0x2C],
         sizeof packet.angular_velocity[0]);
  memcpy(&packet.angular_velocity[1], &bytes[0x30],
         sizeof packet.angular_velocity[1]);
  memcpy(&packet.angular_velocity[2], &bytes[0x34],
         sizeof packet.angular_velocity[2]);

  memcpy(&packet.ride_height, &bytes[0x38], sizeof packet.ride_height);
  memcpy(&packet.engine_rpm, &bytes[0x3C], sizeof packet.engine_rpm);
  memcpy(&packet.iv, &bytes[0x40], sizeof packet.iv);
  memcpy(&packet.fuel_level, &bytes[0x44], sizeof packet.fuel_level);
  memcpy(&packet.fuel_capacity, &bytes[0x48], sizeof packet.fuel_capacity);
  memcpy(&packet.speed, &bytes[0x4C], sizeof packet.speed);
  memcpy(&packet.turbo_boost_pressure, &bytes[0x50],
         sizeof packet.turbo_boost_pressure);
  memcpy(&packet.oil_pressure, &bytes[0x54], sizeof packet.oil_pressure);
  memcpy(&packet.reserved_1, &bytes[0x58], sizeof packet.reserved_1);
  memcpy(&packet.reserved_2, &bytes[0x5C], sizeof packet.reserved_2);

  memcpy(&packet.tyre_temp[0], &bytes[0x60], sizeof packet.tyre_temp[0]);
  memcpy(&packet.tyre_temp[1], &bytes[0x64], sizeof packet.tyre_temp[1]);
  memcpy(&packet.tyre_temp[2], &bytes[0x68], sizeof packet.tyre_temp[2]);
  memcpy(&packet.tyre_temp[3], &bytes[0x6C], sizeof packet.tyre_temp[3]);

  memcpy(&packet.ticks, &bytes[0x70], sizeof packet.ticks);
  memcpy(&packet.current_lap, &bytes[0x74], sizeof packet.current_lap);
  memcpy(&packet.total_laps, &bytes[0x76], sizeof packet.total_laps);
  memcpy(&packet.best_lap_time, &bytes[0x78], sizeof packet.best_lap_time);
  memcpy(&packet.worst_lap_time, &bytes[0x7C], sizeof packet.worst_lap_time);

  memcpy(&packet.day_time_progression, &bytes[0x80],
         sizeof packet.day_time_progression);
  memcpy(&packet.race_position, &bytes[0x84], sizeof packet.race_position);
  memcpy(&packet.num_cars, &bytes[0x86], sizeof packet.num_cars);
  memcpy(&packet.rpm_flashing, &bytes[0x88], sizeof packet.rpm_flashing);
  memcpy(&packet.rpm_rev_limiter, &bytes[0x8A], sizeof packet.rpm_rev_limiter);
  memcpy(&packet.car_max_speed, &bytes[0x8C], sizeof packet.car_max_speed);
  memcpy(&packet.bitflags, &bytes[0x8E], sizeof packet.bitflags);
  memcpy(&packet.gears, &bytes[0x90], sizeof packet.gears);
  memcpy(&packet.throttle, &bytes[0x91], sizeof packet.throttle);
  memcpy(&packet.brake, &bytes[0x92], sizeof packet.brake);
  memcpy(&packet.reversed, &bytes[0x93], sizeof packet.reversed);

  memcpy(&packet.road_plane[0], &bytes[0x94], sizeof packet.road_plane[0]);
  memcpy(&packet.road_plane[1], &bytes[0x98], sizeof packet.road_plane[1]);
  memcpy(&packet.road_plane[2], &bytes[0x9C], sizeof packet.road_plane[2]);
  memcpy(&packet.road_plane[3], &bytes[0xA0], sizeof packet.road_plane[3]);

  memcpy(&packet.wheel_speed[0], &bytes[0xA4], sizeof packet.wheel_speed[0]);
  memcpy(&packet.wheel_speed[1], &bytes[0xA8], sizeof packet.wheel_speed[1]);
  memcpy(&packet.wheel_speed[2], &bytes[0xAC], sizeof packet.wheel_speed[2]);
  memcpy(&packet.wheel_speed[3], &bytes[0xB0], sizeof packet.wheel_speed[3]);

  memcpy(&packet.tyre_radius[0], &bytes[0xB4], sizeof packet.tyre_radius[0]);
  memcpy(&packet.tyre_radius[1], &bytes[0xB8], sizeof packet.tyre_radius[1]);
  memcpy(&packet.tyre_radius[2], &bytes[0xBC], sizeof packet.tyre_radius[2]);
  memcpy(&packet.tyre_radius[3], &bytes[0xC0], sizeof packet.tyre_radius[3]);

  memcpy(&packet.tyre_suspension_travel[0], &bytes[0xC4],
         sizeof packet.tyre_suspension_travel[0]);
  memcpy(&packet.tyre_suspension_travel[1], &bytes[0xC8],
         sizeof packet.tyre_suspension_travel[1]);
  memcpy(&packet.tyre_suspension_travel[2], &bytes[0xCC],
         sizeof packet.tyre_suspension_travel[2]);
  memcpy(&packet.tyre_suspension_travel[3], &bytes[0xD0],
         sizeof packet.tyre_suspension_travel[3]);

  memcpy(&packet.reserved_3, &bytes[0xD4], 32);

  memcpy(&packet.clutch, &bytes[0xF4], sizeof packet.clutch);
  memcpy(&packet.clutch_engagement, &bytes[0xF8], sizeof packet.clutch);
  memcpy(&packet.rpm_through_clutch, &bytes[0xFC], sizeof packet.clutch);
  memcpy(&packet.transmission_top_speed, &bytes[0x100], sizeof packet.clutch);
  memcpy(&packet.gear_ratios[0], &bytes[0x104], sizeof packet.gear_ratios[0]);
  memcpy(&packet.gear_ratios[1], &bytes[0x108], sizeof packet.gear_ratios[1]);
  memcpy(&packet.gear_ratios[2], &bytes[0x10C], sizeof packet.gear_ratios[2]);
  memcpy(&packet.gear_ratios[3], &bytes[0x110], sizeof packet.gear_ratios[3]);
  memcpy(&packet.gear_ratios[4], &bytes[0x114], sizeof packet.gear_ratios[4]);
  memcpy(&packet.gear_ratios[5], &bytes[0x118], sizeof packet.gear_ratios[5]);
  memcpy(&packet.gear_ratios[6], &bytes[0x11C], sizeof packet.gear_ratios[6]);
  memcpy(&packet.gear_ratios[7], &bytes[0x120], sizeof packet.gear_ratios[7]);
  memcpy(&packet.clutch, &bytes[0x124], sizeof packet.clutch);

  return packet;
}
