#include <cstdint>
#include <cstring>
#include <iostream>

struct GT7Packet {
  /*
    float vectors of length 3 are typically xyz coordinates.
    float vectors of length 4 are typically FL/FR/RL/RR tyre values.
  */
  float position[3]; // y is up, xz is track plane; origin varies by track

  float velocity[3]; // m/s

  float rotation_im_i; // Im(i) of unit quaternion (i, j, k) relative to track
  float rotation_im_j; // Im(j) of unit quaternion (i, j, k) relative to track
  float rotation_im_k; // Im(k) of unit quaternion (i, j, k) relative to track
  float rotation_real; // Re part of unit quaternion relative to track coords

  float angular_velocity[3]; // rad/sec

  float ride_height; // may include dynamic suspension
  float engine_rpm;  // rev/min

  char iv[4]; // salsa20 iv

  float fuel_level;           // 0-100, percent level
  float fuel_capacity;        // 5-100, Liters
  float speed;                // m/s, positive when going backwards
  float turbo_boost_pressure; // subtract 1 to yield the turbo boost display
  float oil_pressure;         // in bar?

  float reserved_1; // seems to be always at 85.0, maybe water temp.
  float reserved_2; // seems to be always at 110.0, maybe oil temp.

  float tyre_temp[4];        // ˚C
  std::uint32_t ticks;       // packet number, increases even when paused
  std::uint16_t current_lap; // Which lap we are in, 0 before starting first lap
  std::uint16_t total_laps;  // How many laps the race will have, 0 for TTs
  std::uint32_t best_lap_time;        // millis timestamp
  std::uint32_t worst_lap_time;       // millis timestamp
  std::uint32_t day_time_progression; // millis timestamp, time of day indicates
                                      // race start time of day, affected by
                                      // Variable Time Speed Ratio, useless for
                                      // timing when time speed ratio is not 1
  std::uint16_t race_position;        // position of the car, -1 after start
  std::uint16_t num_cars;             // total number of cars, -1 after start
  std::uint16_t rpm_flashing;    // in RPM, indicates when rev limiter flashes
  std::uint16_t rpm_rev_limiter; // in RPM, indicates when rev limiter is hit
  std::uint16_t car_max_speed;   // in speed units (in-game setting)

  std::uint16_t bitflags; // each bit represents a condition:
                          // 0: in race
                          // 1: paused
                          // 2: loading/processing
                          // 3: in a gear, 0 when shifting or out of gear
                          // 4: has turbo
                          // 5: rev limiter flashing
                          // 6: handbrake
                          // 7: lights
                          // 8: lowbeam
                          // 9: highbeam
                          // 10: ASM
                          // 11: TCS
                          // 12:?
                          // 13:?
                          // 14:?
                          // 15:?

  std::uint16_t gears;    // split into two nibbles/4 bit ints
                          // [0-4] current gear, 0 reverse, -1 neutral
                          // [5-8] suggested gear, -1 none suggested
  std::uint16_t throttle; // 0-255
  std::uint16_t brake;    // 0-255
  std::uint16_t reversed; // always set to 0

  float road_plane[4]; // coefficients of plane equation of the road below? or
                       // nearest? to the car, first three floats are the normal
                       // of the plane, distance of Position to this plane
                       // matches Ride Height
  float wheel_speed[4]; // angular speed in radians / second, sign opposite to
                        // vehicle speed
  float tyre_radius[4]; // in meters, multiply with Wheel Speed to get Tire
                        // Speed in meters/second
  float tyre_suspension_travel[4];

  unsigned char reserved_3[32]; // dunno what this is, always set to 0

  float clutch;
  float clutch_engagement;
  float rpm_through_clutch;
  float transmission_top_speed;
  float gear_ratios[8];
  float car_code;
};

extern void parse_nonce(unsigned char *nonce, unsigned char *packet);

extern GT7Packet parse_bytes(unsigned char *packet);
